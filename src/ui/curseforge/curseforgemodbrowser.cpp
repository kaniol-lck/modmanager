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

#include "curseforge/curseforgemanager.h"
#include "ui/downloadpathselectmenu.h"
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
    manager_(new CurseforgeManager(this, sectionId)),
    proxyModel_(new CurseforgeManagerProxyModel(this)),
    sectionId_(sectionId),
    infoWidget_(new CurseforgeModInfoWidget(this)),
    fileListWidget_(new CurseforgeFileListWidget(this)),
    localMod_(mod)
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    ui->menu_Curseforge->insertActions(ui->menu_Curseforge->actions().first(), menu_->actions());
    proxyModel_->setSourceModel(manager_->model());
    initUi(proxyModel_);

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    if(Config().getShowCatVerInToolBar()){
        ui->toolBar->addMenu(ui->menuSelect_Category);
        ui->toolBar->addMenu(ui->menuSelect_Game_Version);
    }
    ui->toolBar->addMenu(downloadPathSelectMenu_);
    ui->toolBar->addAction(refreshAction_);
    ui->toolBar->addAction(visitWebsiteAction_);
    ui->toolBar->addAction(openDialogAction_);

    ui->searchBar->addWidget(ui->label_3);
    ui->searchBar->addWidget(ui->loaderSelect);
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
        if(action->data().toInt() == sectionId)
            action->setChecked(true);

    updateVersionList();
    updateCategoryList(CurseforgeAPI::cachedSectionCategories(sectionId));
    sectionCategoriesGetter_ = CurseforgeAPI::api()->getSectionCategories(sectionId).asUnique();
    sectionCategoriesGetter_->setOnFinished(this, [=](const auto &list){ updateCategoryList(list); });
    updateStatusText();

    connect(ui->searchText, &QLineEdit::returnPressed, this, &CurseforgeModBrowser::search);
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeModBrowser::updateVersionList);

    connect(manager_, &CurseforgeManager::searchStarted, this, [=]{
        setCursor(Qt::BusyCursor);
        statusBarWidget_->setText(tr("Searching mods..."));
        statusBarWidget_->setProgressVisible(true);
        refreshAction_->setEnabled(false);
    });
    connect(manager_, &CurseforgeManager::searchFinished, this, [=](bool success){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText(success? "" : tr("Failed loading"));
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
        updateStatusText();
        proxyModel_->invalidate();
    });
    connect(manager_, &CurseforgeManager::scrollToTop, this, [=]{
        scrollToTop();
    });

    if(localMod_){
        ui->searchText->setText(localMod_->commonInfo()->id());
    } else if(Config().getSearchModsOnStartup()){
        inited_ = true;
        search();
    }
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
}

void CurseforgeModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        search();
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

void CurseforgeModBrowser::searchModByPathInfo(LocalModPath *path)
{
    currentGameVersion_ = path->info().gameVersion();
    ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(path->info().gameVersion()));
    currentLoaderType_ = path->info().loaderType();
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(path->info().loaderType()));
    downloadPathSelectMenu_->setDownloadPath(path);
    search();
}

void CurseforgeModBrowser::updateUi()
{
    for(auto &&widget : findChildren<CurseforgeModItemWidget *>())
        widget->updateUi();
}

void CurseforgeModBrowser::switchSection()
{
    currentCategoryId_ = 0;
    search();
    updateCategoryList(CurseforgeAPI::cachedSectionCategories(sectionId_));
    sectionCategoriesGetter_ = CurseforgeAPI::api()->getSectionCategories(sectionId_).asUnique();
    sectionCategoriesGetter_->setOnFinished(this, [=](const auto &list){ updateCategoryList(list); });
}

void CurseforgeModBrowser::updateVersionList()
{
    ui->versionToolBar->clear();
    ui->menuSelect_Game_Version->clear();
    ui->menuSelect_Game_Version->setStyleSheet("QMenu { menu-scrollable: 1; }");
    auto anyVersionAction = ui->menuSelect_Game_Version->addAction(tr("Any"));
    connect(anyVersionAction, &QAction::triggered, this, [=]{
        currentGameVersion_ = GameVersion::Any;
        ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(tr("Any")));
        ui->menuSelect_Game_Version->setIcon(QIcon());
        search();
    });
    ui->menuSelect_Game_Version->addSeparator();
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
                ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(version));
                search();
            });
        }
    }
    for(const auto &key : qAsConst(keys)){
        auto submenu = submenus[key];
        if(submenu->actions().size() == 1)
            ui->menuSelect_Game_Version->addActions(submenu->actions());
        else
            ui->menuSelect_Game_Version->addMenu(submenu);
    }
    for(auto &&action : ui->menuSelect_Game_Version->actions()){
        if(action->isSeparator()) continue;
        auto button = new QToolButton;
        button->setDefaultAction(action);
        button->setPopupMode(QToolButton::InstantPopup);
        ui->versionToolBar->addWidget(button);
    }
}

void CurseforgeModBrowser::updateCategoryList(QList<CurseforgeCategoryInfo> list)
{
    ui->categoryToolBar->clear();
    ui->menuSelect_Category->clear();
    auto anyCategoryAction = ui->menuSelect_Category->addAction(tr("Any"));
    connect(anyCategoryAction, &QAction::triggered, this, [=]{
        currentCategoryId_ = 0;
        ui->menuSelect_Category->setTitle(tr("Category : %1").arg(tr("Any")));
        ui->menuSelect_Category->setIcon(QIcon());
        search();
    });
    ui->menuSelect_Category->addSeparator();
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
            ui->menuSelect_Category->setTitle(tr("Category : %1").arg(name));
            ui->menuSelect_Category->setIcon(action->icon());
            search();
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
    ui->menuSelect_Category->addActions(actions.values());
    for(auto &&action : ui->menuSelect_Category->actions()){
        if(action->isSeparator()) continue;
        auto button = new QToolButton;
        button->setDefaultAction(action);
        button->setPopupMode(QToolButton::InstantPopup);
        ui->categoryToolBar->addWidget(button);
    }
}

void CurseforgeModBrowser::search()
{
    manager_->search(ui->searchText->text(),
                     currentCategoryId_,
                     currentGameVersion_,
                     ui->sortSelect->currentIndex());
}

void CurseforgeModBrowser::updateStatusText()
{
    statusBarWidget_->setModCount(manager_->mods().size());
}

QDialog *CurseforgeModBrowser::getDialog(const QModelIndex &index)
{
    auto mod = index.data(Qt::UserRole + 1).value<CurseforgeMod*>();
    if(mod && mod->parent() == manager_){
        auto dialog = new CurseforgeModDialog(this, mod);
        //set parent
        mod->setParent(dialog);
        connect(dialog, &CurseforgeModDialog::finished, this, [=]{
            if(manager_->mods().contains(mod))
                mod->setParent(manager_);
        });
        return dialog;
    } else
        return nullptr;
}

void CurseforgeModBrowser::on_loaderSelect_currentIndexChanged(int index)
{
    proxyModel_->setLoaderType(ModLoaderType::curseforge.at(index));
    proxyModel_->invalidate();
}

void CurseforgeModBrowser::onSelectedItemChanged(const QModelIndex &index)
{
    if(index.isValid())
        selectedMod_ = index.data(Qt::UserRole + 1).value<CurseforgeMod*>();
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
    if(refreshAction_->isEnabled())
        manager_->searchMore();
}

QWidget *CurseforgeModBrowser::getIndexWidget(const QModelIndex &index)
{
    auto mod = index.data(Qt::UserRole + 1).value<CurseforgeMod*>();
    if(mod){
        auto fileInfo = mod->modInfo().latestFileInfo(currentGameVersion_, currentLoaderType_);
        auto widget = new CurseforgeModItemWidget(this, mod, fileInfo);
        manager_->model()->setItemHeight(widget->height());
        return widget;
    } else
        return nullptr;
}

CurseforgeMod *CurseforgeModBrowser::selectedMod() const
{
//    qDebug() << selectedMod_;
    return selectedMod_;
}

ExploreBrowser *CurseforgeModBrowser::another()
{
    return new CurseforgeModBrowser;
}

void CurseforgeModBrowser::on_sortSelect_currentIndexChanged(int)
{
    search();
}

void CurseforgeModBrowser::on_actionMod_triggered()
{
    sectionId_ = CurseforgeAPI::Mod;
    manager_->setSectionId(CurseforgeAPI::Mod);
    switchSection();
}

void CurseforgeModBrowser::on_actionWorld_triggered()
{
    sectionId_ = CurseforgeAPI::World;
    manager_->setSectionId(CurseforgeAPI::World);
    switchSection();
}

void CurseforgeModBrowser::on_actionModpacks_triggered()
{
    sectionId_ = CurseforgeAPI::Modpack;
    manager_->setSectionId(CurseforgeAPI::Modpack);
    switchSection();
}

void CurseforgeModBrowser::on_actionTexturepacks_triggered()
{
    sectionId_ = CurseforgeAPI::TexturePack;
    manager_->setSectionId(CurseforgeAPI::TexturePack);
    switchSection();
}

void CurseforgeModBrowser::on_menuDownload_aboutToShow()
{
    ui->menuDownload->clear();
    ui->menuDownload->setStyleSheet("QMenu { menu-scrollable: 1; }");
    if(!selectedMod_) return;
    auto files = selectedMod_->modInfo().allFileList().isEmpty()?
                selectedMod_->modInfo().latestFiles() : selectedMod_->modInfo().allFileList();
    std::sort(files.begin(), files.end(), [=](const auto &file1, const auto &file2){
        return file1.fileDate() > file2.fileDate();
    });
    for(auto &&file : files){
        ui->menuDownload->addAction(file.displayName() + " ("+ sizeConvert(file.size()) + ")", this, [=]{
            selectedMod_->download(file, downloadPath());
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
    if(selectedMod_ && selectedMod_->parent() == manager_){
        auto dialog = new CurseforgeModDialog(this, selectedMod_);
        //set parent
        selectedMod_->setParent(dialog);
        connect(dialog, &CurseforgeModDialog::finished, this, [=, mod = selectedMod_]{
            if(manager_->mods().contains(mod))
                mod->setParent(manager_);
        });
        dialog->show();
    }
}

QMenu *CurseforgeModBrowser::getCustomContextMenu()
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
