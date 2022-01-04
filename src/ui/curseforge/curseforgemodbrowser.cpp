#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QDebug>
#include <QAction>
#include <QClipboard>
#include <QMenu>
#include <QStandardItem>
#include <QStatusBar>
#include <QActionGroup>
#include <QDesktopServices>

#include "curseforgemodinfowidget.h"
#include "curseforgefilelistwidget.h"
#include "ui/explorestatusbarwidget.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgeapi.h"
#include "curseforgemoditemwidget.h"
#include "curseforgemoddialog.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "download/assetcache.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent, LocalMod *mod, CurseforgeAPI::Section sectionId) :
    ExploreBrowser(parent, QIcon(":/image/curseforge.svg"), "Curseforge", QUrl("https://www.curseforge.com/minecraft/mc-mods")),
    ui(new Ui::CurseforgeModBrowser),
    sectionId_(sectionId),
    infoWidget_(new CurseforgeModInfoWidget(this)),
    fileListWidget_(new CurseforgeFileListWidget(this)),
    api_(new CurseforgeAPI(this)),
    localMod_(mod)
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    ui->menu_Path->insertActions(ui->menu_Path->actions().first(), pathMenu_->actions());
    initUi(ui->downloadPathSelect);

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->versionSelectButton);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_5);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->categorySelectButton);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_3);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->loaderSelect);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->label_4);
    ui->toolBar->insertWidget(ui->actionOpen_Folder, ui->downloadPathSelect);

    ui->searchBar->addWidget(ui->searchText);
    ui->searchBar->addWidget(ui->sortSelect);

    ui->loaderSelect->blockSignals(true);
    for(const auto &type : ModLoaderType::curseforge)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));
    ui->loaderSelect->blockSignals(false);

    auto actionGroup = new QActionGroup(this);
    actionGroup->addAction(ui->actionMod);
    actionGroup->addAction(ui->actionModpacks);
    actionGroup->addAction(ui->actionTexturepacks);
    actionGroup->addAction(ui->actionWorld);
    ui->actionMod->setData(CurseforgeAPI::Mod);
    ui->actionModpacks->setData(CurseforgeAPI::Modpack);
    ui->actionTexturepacks->setData(CurseforgeAPI::TexturePack);
    ui->actionWorld->setData(CurseforgeAPI::World);
    for(auto &&action : actionGroup->actions())
        if(action->data().toInt() == sectionId_)
            action->setChecked(true);

    updateVersionList();
    connect(this, &QObject::destroyed, this, disconnecter(
                CurseforgeAPI::api()->getSectionCategories(sectionId_, [=](const auto &list){ updateCategoryList(list); })));
    updateStatusText();

    connect(ui->searchText, &QLineEdit::returnPressed, this, &CurseforgeModBrowser::search);
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeModBrowser::updateVersionList);
    connect(this, &CurseforgeModBrowser::downloadPathChanged, fileListWidget_, &CurseforgeFileListWidget::setDownloadPath);

    if(localMod_){
        currentName_ = localMod_->commonInfo()->id();
        ui->searchText->setText(currentName_);
    } else if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList(currentName_);
    }
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
    for(auto row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<CurseforgeMod*>();
        if(mod && !mod->parent())
            mod->deleteLater();
    }
}

void CurseforgeModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        getModList(currentName_);
    }
}

QWidget *CurseforgeModBrowser::infoWidget() const
{
    return infoWidget_;
}

QWidget *CurseforgeModBrowser::fileListWidget() const
{
    return fileListWidget_;
}

void CurseforgeModBrowser::refresh()
{
    search();
}

void CurseforgeModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    currentGameVersion_ = info.gameVersion();
    ui->versionSelectButton->setText(info.gameVersion());
    currentLoaderType_ = info.loaderType();
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    ui->downloadPathSelect->setCurrentText(info.displayName());
    getModList(currentName_);
}

void CurseforgeModBrowser::updateUi()
{
    for(auto &&widget : findChildren<CurseforgeModItemWidget *>())
        widget->updateUi();
}

void CurseforgeModBrowser::updateVersionList()
{
    ui->versionToolBar->clear();
    auto menu = new QMenu;
    menu->setStyleSheet("QMenu { menu-scrollable: 1; }");
    auto anyVersionAction = menu->addAction(tr("Any"));
    connect(anyVersionAction, &QAction::triggered, this, [=]{
        currentGameVersion_ = GameVersion::Any;
        ui->versionSelectButton->setText(tr("Any"));
        ui->versionSelectButton->setIcon(QIcon());
    });
    anyVersionAction->trigger();
    menu->addSeparator();
    QMap<QString, QMenu*> submenus;
    QList<QString> keys; //to keep order
    for(auto &&version : GameVersion::curseforgeVersionList()){
        if(!submenus.contains(version.majorVersion())){
            auto submenu = new QMenu(version.majorVersion());
            submenus[version.majorVersion()] = submenu;
            keys << version.majorVersion();
        }
    }
    for(auto &&version : GameVersion::curseforgeVersionList()){
        if(submenus.contains(version.majorVersion())){
            auto submenu = submenus[version.majorVersion()];
            if(submenu->actions().size() == 1)
                if(GameVersion version = submenu->actions().at(0)->text(); version == version.majorVersion())
                    submenu->addSeparator();
            submenu->addAction(version, this, [=]{
                currentGameVersion_ = version;
                ui->versionSelectButton->setText(version);
                getModList(currentName_);
            });
        }
    }
    for(const auto &key : qAsConst(keys)){
        auto submenu = submenus[key];
        if(submenu->actions().size() == 1)
            menu->addActions(submenu->actions());
        else
            menu->addMenu(submenu);
    }
    for(auto &&action : menu->actions()){
        if(action->isSeparator()) continue;
        auto button = new QToolButton;
        button->setDefaultAction(action);
        button->setPopupMode(QToolButton::InstantPopup);
        ui->versionToolBar->addWidget(button);
    }
    ui->versionSelectButton->setMenu(menu);
}

void CurseforgeModBrowser::updateCategoryList(QList<CurseforgeCategoryInfo> list)
{
    ui->categoryToolBar->clear();
    auto menu = new QMenu;
    auto anyCategoryAction = menu->addAction(tr("Any"));
    connect(anyCategoryAction, &QAction::triggered, this, [=]{
        currentCategoryId_ = 0;
        ui->categorySelectButton->setText(tr("Any"));
        ui->categorySelectButton->setIcon(QIcon());
    });
    anyCategoryAction->trigger();
    menu->addSeparator();
    std::sort(list.begin(), list.end(), [=](const auto &info1, const auto &info2){
        return info1.id() < info2.id();
    });
    QMap<int, QAction *> actions;
    for(auto &&info : list){
        auto iconAsset = new AssetCache(this, info.avatarUrl(), info.avatarUrl().fileName(), CurseforgeCategoryInfo::cachePath());
        bool exists = iconAsset->exists();
        QString name = info.name();
        name.replace("&", "&&");
        auto action = new QAction(info.name());
        if(exists){
            action->setIcon(QIcon(iconAsset->destFilePath()));
        } else{
            iconAsset->download();
            connect(iconAsset, &AssetCache::assetReady, this, [=]{
                action->setIcon(QIcon(iconAsset->destFilePath()));
            });
        }
        connect(action, &QAction::triggered, this, [=]{
            currentCategoryId_ = info.id();
            ui->categorySelectButton->setText(name);
            ui->categorySelectButton->setIcon(action->icon());
            getModList(currentName_);
        });
        if(auto parentId = info.parentGameCategoryId(); parentId == sectionId_){
            actions[info.id()] = action;
        } else if(actions.contains(parentId)){
            if(actions[parentId]->menu())
                actions[parentId]->menu()->addAction(action);
            else {
                auto parentAction = new QAction(actions[parentId]->icon(), actions[parentId]->text());
                parentAction->setMenu(new QMenu);
                parentAction->menu()->addAction(actions[parentId]);
                parentAction->menu()->addSeparator();
                parentAction->menu()->addAction(action);
                actions[parentId] = parentAction;
                if(!exists)
                    connect(iconAsset, &AssetCache::assetReady, this, [=]{
                        parentAction->setIcon(QIcon(iconAsset->destFilePath()));
                    });
            }
        }
    }
    menu->addActions(actions.values());
    for(auto &&action : menu->actions()){
        if(action->isSeparator()) continue;
        auto button = new QToolButton;
        button->setDefaultAction(action);
        button->setPopupMode(QToolButton::InstantPopup);
        ui->categoryToolBar->addWidget(button);
    }
    ui->categorySelectButton->setMenu(menu);
}

void CurseforgeModBrowser::search()
{
//    if(ui->searchText->text() == currentName_) return;
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void CurseforgeModBrowser::updateStatusText()
{
    QString sectionStr;
    switch (sectionId_) {
    case CurseforgeAPI::BukkitPlugin:
        break;
    case CurseforgeAPI::Mod:
        sectionStr = tr("mods");
        break;
    case CurseforgeAPI::TexturePack:
        sectionStr = tr("texturepacks");
        break;
    case CurseforgeAPI::World:
        sectionStr = tr("worlds");
        break;
    case CurseforgeAPI::Modpack:
        sectionStr = tr("modpacks");
        break;
    case CurseforgeAPI::Addon:
        break;
    case CurseforgeAPI::Customization:
        break;
    }
    auto str = tr("Loaded %1 %2 from Curseforge.").arg(model_->rowCount() - (hasMore_? 0 : 1)).arg(sectionStr);
    ui->statusbar->showMessage(str);
}

void CurseforgeModBrowser::getModList(QString name, int index, int needMore)
{
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    GameVersion gameVersion = currentGameVersion_;
    auto category = currentCategoryId_;
    auto sort = ui->sortSelect->currentIndex();

    isSearching_ = true;
    auto conn = api_->searchMods(sectionId_, gameVersion, index, name, category, sort, [=](const QList<CurseforgeModInfo> &infoList){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);

        //new search
        if(currentIndex_ == 0){
            idList_.clear();
            for(auto row = 0; row < model_->rowCount(); row++){
                auto item = model_->item(row);
                auto mod = item->data().value<CurseforgeMod*>();
                if(mod && !mod->parent())
                    mod->deleteLater();
            }
            model_->clear();
            hasMore_ = true;
        }

        //show them
        int shownCount = 0;
        for(const auto &info : qAsConst(infoList)){
            //do not load duplicate mod
            if(idList_.contains(info.id()))
                continue;
            idList_ << info.id();

            auto mod = new CurseforgeMod((QObject*)nullptr, info);
            auto *item = new QStandardItem();
            item->setData(QVariant::fromValue(mod));
            auto fileInfo = mod->modInfo().latestFileInfo(currentGameVersion_, currentLoaderType_);
            item->setSizeHint(QSize(0, 100));
            model_->appendRow(item);
            auto isShown = currentLoaderType_ == ModLoaderType::Any || info.loaderTypes().contains(currentLoaderType_);
            if(info.loaderTypes().isEmpty())
                isShown = true;
            setRowHidden(item->row(), !isShown);
            if(isShown){
                shownCount++;
                mod->acquireIcon();
            }
        }
        if(infoList.size() < Config().getSearchResultCount()){
            auto item = new QStandardItem(tr("There is no more mod here..."));
            item->setSizeHint(QSize(0, 108));
            auto font = qApp->font();
            font.setPointSize(20);
            item->setFont(font);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            model_->appendRow(item);
            hasMore_ = false;
        }
        isSearching_ = false;
        if(hasMore_ && shownCount != infoList.count() && shownCount < needMore){
            currentIndex_ += 20;
            getModList(currentName_, currentIndex_, needMore - shownCount);
        }
        updateStatusText();
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

QDialog *CurseforgeModBrowser::getDialog(QStandardItem *item)
{
    auto mod = item->data().value<CurseforgeMod*>();
    if(mod && !mod->parent()){
        auto dialog = new CurseforgeModDialog(this, mod);
        //set parent
        mod->setParent(dialog);
        dialog->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeModBrowser::downloadPathChanged, dialog, &CurseforgeModDialog::setDownloadPath);
        connect(dialog, &CurseforgeModDialog::finished, this, [=]{
            mod->setParent(nullptr);
        });
        return dialog;
    } else
        return nullptr;
}

void CurseforgeModBrowser::on_loaderSelect_currentIndexChanged(int index)
{
    currentLoaderType_ = ModLoaderType::curseforge.at(index);
    for(int row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<CurseforgeMod*>();
        if(!mod) continue;
        auto isShown = currentLoaderType_ == ModLoaderType::Any || mod->modInfo().loaderTypes().contains(currentLoaderType_);
        if(mod->modInfo().loaderTypes().isEmpty())
            isShown = true;
        bool isHidden = isRowHidden(row);
        setRowHidden(row, !isShown);
        //hidden -> shown, while not have downloaded thumbnail yet
        if(isHidden && isShown && mod->modInfo().icon().isNull())
            mod->acquireIcon();
    }
}

void CurseforgeModBrowser::onSelectedItemChanged(QStandardItem *item)
{
    if(item)
        selectedMod_ = item->data().value<CurseforgeMod*>();
    else
        selectedMod_ = nullptr;
    ui->actionOpen_Curseforge_Mod_Dialog->setEnabled(selectedMod_);
    ui->menuDownload->setEnabled(selectedMod_);
    ui->actionCopy_Website_Link->setEnabled(selectedMod_);
    ui->actionOpen_Website_Link->setEnabled(selectedMod_);
    emit selectedModsChanged(selectedMod_);
    infoWidget_->setMod(selectedMod_);
    fileListWidget_->setMod(selectedMod_);
}

void CurseforgeModBrowser::loadMore()
{
    if(!isSearching_ && hasMore_){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

QWidget *CurseforgeModBrowser::getIndexWidget(QStandardItem *item)
{
    auto mod = item->data().value<CurseforgeMod*>();
    if(mod){
        auto fileInfo = mod->modInfo().latestFileInfo(currentGameVersion_, currentLoaderType_);
        auto widget = new CurseforgeModItemWidget(nullptr, mod, fileInfo);
        widget->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeModBrowser::downloadPathChanged, widget, &CurseforgeModItemWidget::setDownloadPath);
        return widget;
    } else
        return nullptr;
}

CurseforgeMod *CurseforgeModBrowser::selectedMod() const
{
//    qDebug() << selectedMod_;
    return selectedMod_;
}

QList<QAction *> CurseforgeModBrowser::modActions() const
{
    return ui->menu_Mod->actions();
}

QList<QAction *> CurseforgeModBrowser::pathActions() const
{
    return ui->menu_Path->actions();
}

ExploreBrowser *CurseforgeModBrowser::another()
{
    return new CurseforgeModBrowser;
}

void CurseforgeModBrowser::on_sortSelect_currentIndexChanged(int)
{
    getModList(currentName_);
}

void CurseforgeModBrowser::on_actionOpen_Folder_triggered()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void CurseforgeModBrowser::on_actionMod_triggered()
{
    sectionId_ = CurseforgeAPI::Mod;
    currentCategoryId_ = 0;
    search();
    connect(this, &QObject::destroyed, this, disconnecter(
                CurseforgeAPI::api()->getSectionCategories(sectionId_, [=](const auto &list){ updateCategoryList(list); })));

}

void CurseforgeModBrowser::on_actionWorld_triggered()
{
    sectionId_ = CurseforgeAPI::World;
    currentCategoryId_ = 0;
    search();
    connect(this, &QObject::destroyed, this, disconnecter(
                CurseforgeAPI::api()->getSectionCategories(sectionId_, [=](const auto &list){ updateCategoryList(list); })));

}

void CurseforgeModBrowser::on_actionModpacks_triggered()
{
    sectionId_ = CurseforgeAPI::Modpack;
    currentCategoryId_ = 0;
    search();
    connect(this, &QObject::destroyed, this, disconnecter(
                CurseforgeAPI::api()->getSectionCategories(sectionId_, [=](const auto &list){ updateCategoryList(list); })));

}

void CurseforgeModBrowser::on_actionTexturepacks_triggered()
{
    sectionId_ = CurseforgeAPI::TexturePack;
    currentCategoryId_ = 0;
    search();
    connect(this, &QObject::destroyed, this, disconnecter(
                CurseforgeAPI::api()->getSectionCategories(sectionId_, [=](const auto &list){ updateCategoryList(list); })));

}

void CurseforgeModBrowser::on_menuDownload_aboutToShow()
{
    ui->menuDownload->clear();
    ui->menuDownload->setStyleSheet("QMenu { menu-scrollable: 1; }");
    if(!selectedMod_) return;
    auto &&files = selectedMod_->modInfo().allFileList().isEmpty()?
                selectedMod_->modInfo().latestFiles() : selectedMod_->modInfo().allFileList();
    for(auto &&file : files){
        ui->menuDownload->addAction(file.displayName() + " ("+ sizeConvert(file.size()) + ")", this, [=]{
            selectedMod_->download(file, downloadPath_);
        });
    }
}

void CurseforgeModBrowser::on_actionCopy_Website_Link_triggered()
{
    if(!selectedMod_) return;
    QApplication::clipboard()->setText(selectedMod_->modInfo().websiteUrl().toString());
}

void CurseforgeModBrowser::on_actionOpen_Curseforge_Mod_Dialog_triggered()
{
    if(!selectedMod_) return;
    if(selectedMod_ && !selectedMod_->parent()){
        auto dialog = new CurseforgeModDialog(this, selectedMod_);
        //set parent
        selectedMod_->setParent(dialog);
        dialog->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeModBrowser::downloadPathChanged, dialog, &CurseforgeModDialog::setDownloadPath);
        connect(dialog, &CurseforgeModDialog::finished, this, [=]{
            selectedMod_->setParent(nullptr);
        });
        dialog->show();
    }
}

QMenu *CurseforgeModBrowser::getMenu()
{
    auto menu = new QMenu(this);
    menu->addAction(ui->actionOpen_Curseforge_Mod_Dialog);
    menu->addMenu(ui->menuDownload);
    menu->addAction(ui->actionCopy_Website_Link);
    menu->addAction(ui->actionOpen_Website_Link);
    return menu;
}

void CurseforgeModBrowser::on_actionOpen_Website_Link_triggered()
{
    if(!selectedMod_) return;
    QDesktopServices::openUrl(selectedMod_->modInfo().websiteUrl());
}
