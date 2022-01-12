#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QMenu>
#include <QClipboard>
#include <QStandardItem>
#include <QDesktopServices>

#include "ui/downloadpathselectmenu.h"
#include "modrinthmodinfowidget.h"
#include "modrinthfilelistwidget.h"
#include "ui/explorestatusbarwidget.h"
#include "modrinthmoditemwidget.h"
#include "modrinthmoddialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "modrinth/modrinthmod.h"
#include "modrinth/modrinthapi.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "util/unclosedmenu.h"
#include "local/localmod.h"

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent, LocalMod *localMod) :
    ExploreBrowser(parent, QIcon(":/image/modrinth.svg"), "Modrinth", QUrl("https://modrinth.com/mods")),
    ui(new Ui::ModrinthModBrowser),
    infoWidget_(new ModrinthModInfoWidget(this)),
    fileListWidget_(new ModrinthFileListWidget(this)),
    api_(new ModrinthAPI(this)),
    localMod_(localMod)
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    ui->menu_Path->insertActions(ui->menu_Path->actions().first(), pathMenu_->actions());
    initUi();

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    ui->toolBar->addMenu(ui->menuSelect_Game_Version);
    ui->toolBar->addMenu(ui->menuSelect_Category);
    ui->toolBar->addWidget(ui->label_3);
    ui->toolBar->addWidget(ui->loaderSelect);
    ui->toolBar->addMenu(downloadPathSelectMenu_);

    ui->searchBar->addWidget(ui->searchText);
    ui->searchBar->addWidget(ui->sortSelect);

    ui->loaderSelect->blockSignals(true);
    for(const auto &type : ModLoaderType::modrinth)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));
    ui->loaderSelect->blockSignals(false);

    updateVersionList();
    updateCategoryList();
    updateStatusText();

    connect(ui->searchText, &QLineEdit::returnPressed, this, &ModrinthModBrowser::search);
    connect(VersionManager::manager(), &VersionManager::modrinthVersionListUpdated, this, &ModrinthModBrowser::updateVersionList);

    if(localMod_){
        currentName_ = localMod_->commonInfo()->id();
        ui->searchText->setText(currentName_);
    } else if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList(currentName_);
    }
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
    for(auto row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<ModrinthMod*>();
        if(mod && !mod->parent())
            mod->deleteLater();
    }
}

void ModrinthModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        getModList(currentName_);
    }
}

QWidget *ModrinthModBrowser::infoWidget() const
{
    return infoWidget_;
}

QWidget *ModrinthModBrowser::fileListWidget() const
{
    return fileListWidget_;
}

void ModrinthModBrowser::refresh()
{
    search();
}

void ModrinthModBrowser::searchModByPathInfo(LocalModPath *path)
{
    currentGameVersions_ = { path->info().gameVersion() };
    ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(path->info().gameVersion()));
    ui->loaderSelect->blockSignals(true);
    ui->loaderSelect->setCurrentIndex(ModLoaderType::modrinth.indexOf(path->info().loaderType()));
    ui->loaderSelect->blockSignals(false);
    downloadPathSelectMenu_->setDownloadPath(path);
    getModList(currentName_);
}

void ModrinthModBrowser::updateUi()
{
    for(auto &&widget : findChildren<ModrinthModItemWidget *>())
        widget->updateUi();
}

void ModrinthModBrowser::updateVersionList()
{
    ui->versionToolBar->clear();
    ui->menuSelect_Game_Version->clear();
    auto multiSelectionAction = ui->menuSelect_Game_Version->addAction(tr("Multi Selection"));
    multiSelectionAction->setCheckable(true);
    connect(multiSelectionAction, &QAction::toggled, ui->menuSelect_Game_Version, &UnclosedMenu::setUnclosed);
    connect(multiSelectionAction, &QAction::toggled, this, [=](bool checked){
        Config().setModrinthMultiVersion(checked);
        if(!checked && currentGameVersions_.size() > 1){
            auto version = currentGameVersions_.first();
            ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(version));
            currentGameVersions_ = { version };
            lastGameVersions_ = currentGameVersions_;
            getModList(currentName_);
        }
    });
    auto showSnapshotAction = ui->menuSelect_Game_Version->addAction(tr("Show Snapshot"));
    showSnapshotAction->setCheckable(true);
    connect(showSnapshotAction, &QAction::toggled, this, [=](bool checked){
        Config().setShowModrinthSnapshot(checked);
        for(auto &&action : ui->menuSelect_Game_Version->actions()){
            if(action->data().toBool())
                action->setVisible(checked);
            if(auto &&submenu = action->menu()){
                for (auto &&subaction : submenu->actions()) {
                    if(subaction->data().toBool())
                        subaction->setVisible(checked);
                }
            }
        }
    });
    ui->menuSelect_Game_Version->addSeparator();
    auto anyVersionAction = ui->menuSelect_Game_Version->addAction(tr("Any"));
    connect(anyVersionAction, &QAction::triggered, this, [=]{
        currentGameVersions_.clear();
        ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(tr("Any")));
        ui->menuSelect_Game_Version->setIcon(QIcon());
        getModList(currentName_);
    });
    auto getVersionAction = [=](const GameVersion &version){
        auto versionAction = new QAction(version);
        versionAction->setCheckable(multiSelectionAction->isChecked());
        connect(versionAction, &QAction::triggered, this, [=](bool checked){
            if(multiSelectionAction->isChecked()){
                if(checked)
                    currentGameVersions_ << version;
                else
                    currentGameVersions_.removeAll(version);
            }else {
                currentGameVersions_.clear();
                currentGameVersions_ << version;
                //only search for single selection
                if(currentGameVersions_ == lastGameVersions_) return;
                lastGameVersions_ = currentGameVersions_;
            }
            getModList(currentName_);
            ui->menuSelect_Game_Version->setToolTip("");
            if(auto size = currentGameVersions_.size(); size == 0)
                ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(tr("Any")));
            else if(size == 1)
                ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1").arg(version));
            else{
                QStringList list;
                for(const auto &version : qAsConst(currentGameVersions_))
                    list << version;
                ui->menuSelect_Game_Version->setTitle(tr("Game Version : %1 etc.").arg(list.first()));
                ui->menuSelect_Game_Version->setToolTip(list.join(tr(", ")));
            }
        });
        connect(multiSelectionAction, &QAction::triggered, versionAction, &QAction::setCheckable);
        connect(anyVersionAction, &QAction::triggered, versionAction, &QAction::setChecked);
        return versionAction;
    };

    anyVersionAction->trigger();
    ui->menuSelect_Game_Version->addSeparator();
    QMap<QString, QMenu*> submenus;
    QList<QString> keys; //to keep order
    for(auto &&version : GameVersion::modrinthVersionList()){
        QString submenuName;
        QString type = version.type();
        if(!type.isEmpty() && type != "release"){
            if(type == "snapshot" && version.majorVersion() == GameVersion::Any)
                submenuName = tr("Future Version");
            else
                submenuName = type;
        }
        if(version.majorVersion() != GameVersion::Any)
            submenuName = version.majorVersion();
        if(submenuName.isEmpty() || submenus.contains(submenuName)) continue;
        auto submenu = new UnclosedMenu(submenuName);
        if(type != "release")
            submenu->menuAction()->setData(true);
//        submenu->setUnclosed(multiSelectionAction->isChecked());
        connect(multiSelectionAction, &QAction::toggled, submenu, &UnclosedMenu::setUnclosed);
        submenus[submenuName] = submenu;
        keys << submenuName;
    }
    for(auto &&version : GameVersion::modrinthVersionList()){
        QString submenuName;
        QString type = version.type();
        if(!type.isEmpty() && type != "release"){
            if(type == "snapshot" && version.majorVersion() == GameVersion::Any)
                submenuName = tr("Future Version");
            else
                submenuName = type;
        }
        if(version.majorVersion() != GameVersion::Any)
            submenuName = version.majorVersion();
        if(submenus.contains(submenuName)){
            auto submenu = submenus[submenuName];
            if(submenu->actions().size() == 1)
                if(GameVersion version = submenu->actions().at(0)->text(); version.id() == submenuName)
                    submenu->addSeparator();
            auto action = getVersionAction(version);
            if(version.type() != "release")
                action->setData(true);
            submenu->addAction(action);
        }
    }
    for(const auto &key : qAsConst(keys)){
        auto submenu = submenus[key];
        if(submenu->actions().size() == 1)
            ui->menuSelect_Game_Version->addActions(submenu->actions());
        else
            ui->menuSelect_Game_Version->addMenu(submenu);
    }
    multiSelectionAction->setChecked(Config().getModrinthMultiVersion());
    multiSelectionAction->trigger();
    showSnapshotAction->setChecked(Config().getShowModrinthSnapshot());
    showSnapshotAction->trigger();
//    connect(ui->menuSelect_Game_Version, &QMenu::aboutToHide, this, [=,
//            i = ui->menuSelect_Game_Version->actions().indexOf(multiSelectionAction)]{
//        //only search for multi selection
//        if(!ui->menuSelect_Game_Version->actions().at(i)->isChecked()) return;
//        if(currentGameVersions_ == lastGameVersions_) return;
//        lastGameVersions_ = currentGameVersions_;
//        getModList(currentName_);
//    });
    for(auto &&action : ui->menuSelect_Game_Version->actions()){
        if(action->isSeparator()) continue;
        auto button = new QToolButton;
        button->setDefaultAction(action);
        button->setPopupMode(QToolButton::InstantPopup);
        ui->versionToolBar->addWidget(button);
    }
}

void ModrinthModBrowser::updateCategoryList()
{
    ui->categoryToolBar->clear();
    ui->menuSelect_Category->clear();
    auto multiSelectionAction = ui->menuSelect_Category->addAction(tr("Multi Selection"));
    multiSelectionAction->setCheckable(true);
    multiSelectionAction->setChecked(Config().getModrinthMultiCategory());
    ui->menuSelect_Category->setUnclosed(multiSelectionAction->isChecked());
    connect(multiSelectionAction, &QAction::toggled, ui->menuSelect_Category, &UnclosedMenu::setUnclosed);
    connect(multiSelectionAction, &QAction::toggled, this, [=](bool checked){
        Config().setModrinthMultiCategory(checked);
        if(!checked && currentCategoryIds_.size() > 1){
            auto categoryId = currentCategoryIds_.first();
            auto &&categories = ModrinthAPI::getCategories();
            if(auto it = std::find_if(categories.cbegin(), categories.cend(), [=](const auto &val){
                return std::get<1>(val) == categoryId;
            }); it != categories.cend()){
                auto &&[name, id] = *it;
                ui->menuSelect_Category->setTitle(tr("Category : %1").arg(name));
                QIcon icon(QString(":/image/modrinth/%1.svg").arg(id));
                ui->menuSelect_Category->setIcon(icon);
            }
            currentCategoryIds_.clear();
            currentCategoryIds_ << categoryId;
            lastCategoryIds_ = currentCategoryIds_;
            getModList(currentName_);
        }
    });
    ui->menuSelect_Category->addSeparator();
    auto anyCategoryAction = ui->menuSelect_Category->addAction(tr("Any"));
    connect(anyCategoryAction, &QAction::triggered, this, [=]{
        currentCategoryIds_.clear();
        ui->menuSelect_Category->setTitle(tr("Category : %1").arg(tr("Any")));
        ui->menuSelect_Category->setIcon(QIcon());
        getModList(currentName_);
    });
    anyCategoryAction->trigger();
    ui->menuSelect_Category->addSeparator();
    for(auto &&[name, id] : ModrinthAPI::getCategories()){
        QIcon icon(QString(":/image/modrinth/%1.svg").arg(id));
        auto categoryAction = ui->menuSelect_Category->addAction(icon, name);
        categoryAction->setCheckable(multiSelectionAction->isChecked());
        connect(categoryAction, &QAction::triggered, this, [=, id = id, name = name](bool checked){
            if(multiSelectionAction->isChecked()){
                if(checked)
                    currentCategoryIds_ << id;
                else
                    currentCategoryIds_.removeAll(id);
            }else {
                currentCategoryIds_.clear();
                currentCategoryIds_ << id;
                //only search for single selection
                if(currentCategoryIds_ == lastCategoryIds_) return;
                lastCategoryIds_ = currentCategoryIds_;
            }
            getModList(currentName_);
            ui->menuSelect_Category->setIcon(QIcon());
            ui->menuSelect_Category->setToolTip("");
            if(auto size = currentCategoryIds_.size(); size == 0)
                ui->menuSelect_Category->setTitle(tr("Category : %1").arg(tr("Any")));
            else if(size == 1){
                ui->menuSelect_Category->setTitle(tr("Category : %1").arg(name));
                ui->menuSelect_Category->setIcon(icon);
            } else{
                QStringList list;
                for(const auto &categoryId: qAsConst(currentCategoryIds_)){
                    auto &&categories = ModrinthAPI::getCategories();
                    if(auto it = std::find_if(categories.cbegin(), categories.cend(), [=](const auto &val){
                        return std::get<1>(val) == categoryId;
                    }); it != categories.cend())
                        list << std::get<0>(*it);
                }
                ui->menuSelect_Category->setTitle(tr("Category : %1 etc.").arg(list.first()));
                ui->menuSelect_Category->setToolTip(list.join(tr(", ")));
            }
        });
        connect(multiSelectionAction, &QAction::triggered, categoryAction, &QAction::setCheckable);
        connect(anyCategoryAction, &QAction::triggered, categoryAction, &QAction::setChecked);
    }
//    connect(ui->menuSelect_Category, &QMenu::aboutToHide, this, [=,
//            i = ui->menuSelect_Game_Version->actions().indexOf(multiSelectionAction)]{
//        //only search for multi selection
//        if(!ui->menuSelect_Game_Version->actions().at(i)->isChecked()) return;
//        if(currentCategoryIds_ == lastCategoryIds_) return;
//        lastCategoryIds_ = currentCategoryIds_;
//        getModList(currentName_);
//    });
    for(auto &&action : ui->menuSelect_Category->actions()){
        if(action->isSeparator()) continue;
        auto button = new QToolButton;
        button->setDefaultAction(action);
        button->setPopupMode(QToolButton::InstantPopup);
        ui->categoryToolBar->addWidget(button);
    }
}

void ModrinthModBrowser::search()
{
//    if(ui->searchText->text() == currentName_) return;
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void ModrinthModBrowser::updateStatusText()
{
    auto str = tr("Loaded %1 mods from Modrinth.").arg(model_->rowCount() - (hasMore_? 0 : 1));
    ui->statusbar->showMessage(str);
}

void ModrinthModBrowser::getModList(QString name, int index)
{
    if(isSearching_) return;
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_)
        return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    auto sort = ui->sortSelect->currentIndex();
    auto type = ModLoaderType::modrinth.at(ui->loaderSelect->currentIndex());

    isSearching_ = true;
    searchModsGetter_ = api_->searchMods(name, currentIndex_, currentGameVersions_, type, currentCategoryIds_, sort).asUnique();
    searchModsGetter_->setOnFinished(this, [=](const QList<ModrinthModInfo> &infoList){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);

        //new search
        if(currentIndex_ == 0){
            scrollToTop();
            for(auto row = 0; row < model_->rowCount(); row++){
                auto item = model_->item(row);
                auto mod = item->data().value<ModrinthMod*>();
                if(mod && !mod->parent())
                    mod->deleteLater();
            }
            model_->clear();
            hasMore_ = true;
        }

        //show them
        for(const auto &info : qAsConst(infoList)){
            auto mod = new ModrinthMod((QObject*)nullptr, info);
            auto *item = new QStandardItem();
            item->setData(QVariant::fromValue(mod));
            item->setSizeHint(QSize(0, 100));
            model_->appendRow(item);
            mod->acquireIcon();
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
        updateStatusText();
    });
}

QDialog *ModrinthModBrowser::getDialog(QStandardItem *item)
{
    auto mod = item->data().value<ModrinthMod*>();
    if(mod && !mod->parent()){
        auto dialog = new ModrinthModDialog(this, mod);
        //set parent
        mod->setParent(dialog);
        connect(dialog, &ModrinthModDialog::finished, this, [=]{
            mod->setParent(nullptr);
        });
        return dialog;
    }
    return nullptr;
}

void ModrinthModBrowser::on_sortSelect_currentIndexChanged(int)
{
    getModList(currentName_);
}

void ModrinthModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    getModList(currentName_);
}

void ModrinthModBrowser::onSelectedItemChanged(QStandardItem *item)
{
    if(item)
        selectedMod_ = item->data().value<ModrinthMod*>();
    else
        selectedMod_ = nullptr;
    ui->actionOpen_Modrinth_Mod_Dialog->setEnabled(selectedMod_);
    ui->menuDownload->setEnabled(selectedMod_);
    ui->actionCopy_Website_Link->setEnabled(selectedMod_);
    ui->actionOpen_Website_Link->setEnabled(selectedMod_);
    emit selectedModsChanged(selectedMod_);
    infoWidget_->setMod(selectedMod_);
    fileListWidget_->setMod(selectedMod_);
}

QWidget *ModrinthModBrowser::getIndexWidget(QStandardItem *item)
{
    auto mod = item->data().value<ModrinthMod*>();
    if(mod){
        auto widget = new ModrinthModItemWidget(nullptr, mod);
        return widget;
    } else
        return nullptr;
}

void ModrinthModBrowser::loadMore()
{
    if(!isSearching_ && hasMore_){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

ModrinthMod *ModrinthModBrowser::selectedMod() const
{
    return selectedMod_;
}

QList<QAction *> ModrinthModBrowser::modActions() const
{
    return ui->menu_Mod->actions();
}

QList<QAction *> ModrinthModBrowser::pathActions() const
{
    return ui->menu_Path->actions();
}

ExploreBrowser *ModrinthModBrowser::another()
{
    return new ModrinthModBrowser;
}

void ModrinthModBrowser::on_menuDownload_aboutToShow()
{
    ui->menuDownload->clear();
    ui->menuDownload->setStyleSheet("QMenu { menu-scrollable: 1; }");
    if(!selectedMod_) return;
    for(auto &&file : selectedMod_->modInfo().fileList()){
        ui->menuDownload->addAction(file.displayName(), this, [=]{
            selectedMod_->download(file, downloadPath());
        });
    }
}

void ModrinthModBrowser::on_actionCopy_Website_Link_triggered()
{
    if(!selectedMod_) return;
    QApplication::clipboard()->setText(selectedMod_->modInfo().websiteUrl().toString());
}

void ModrinthModBrowser::on_actionOpen_Modrinth_Mod_Dialog_triggered()
{
    if(!selectedMod_) return;
    if(selectedMod_ && !selectedMod_->parent()){
        auto dialog = new ModrinthModDialog(this, selectedMod_);
        //set parent
        selectedMod_->setParent(dialog);
        connect(dialog, &ModrinthModDialog::finished, this, [=]{
            selectedMod_->setParent(nullptr);
        });
        dialog->show();
    }
}

QMenu *ModrinthModBrowser::getMenu()
{
    auto menu = new QMenu(this);
    menu->addAction(ui->actionOpen_Modrinth_Mod_Dialog);
    menu->addMenu(ui->menuDownload);
    menu->addAction(ui->actionCopy_Website_Link);
    menu->addAction(ui->actionOpen_Website_Link);
    return menu;
}

void ModrinthModBrowser::on_actionOpen_Website_Link_triggered()
{
    if(!selectedMod_) return;
    QDesktopServices::openUrl(selectedMod_->modInfo().websiteUrl());
}
