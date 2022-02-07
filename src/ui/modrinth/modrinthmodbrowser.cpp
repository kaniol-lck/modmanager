#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>
#include <QDir>
#include <QMenu>
#include <QClipboard>
#include <QStandardItem>
#include <QDesktopServices>

#include "modrinth/modrinthmanager.h"
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
#include "ui/tagswidget.h"

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent, LocalMod *localMod) :
    ExploreBrowser(parent, QIcon(":/image/modrinth.svg"), "Modrinth", QUrl("https://modrinth.com/mods")),
    ui(new Ui::ModrinthModBrowser),
    manager_(new ModrinthManager(this)),
    infoWidget_(new ModrinthModInfoWidget(this)),
    fileListWidget_(new ModrinthFileListWidget(this)),
    localMod_(localMod)
{
    infoWidget_->hide();
    fileListWidget_->hide();
    ui->setupUi(this);
    ui->menu_Modrinth->insertActions(ui->menu_Modrinth->actions().first(), menu_->actions());
    initUi(manager_);
    treeViewIndexWidgetColumn_ = ModrinthManagerModel::CategoryColumn;

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    if(Config().getShowCatVerInToolBar()){
        ui->toolBar->addMenu(ui->menuSelect_Game_Version);
        ui->toolBar->addMenu(ui->menuSelect_Category);
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
    for(const auto &type : ModLoaderType::modrinth)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));
    ui->loaderSelect->blockSignals(false);

    updateVersionList();
    updateCategoryList();
    updateStatusText();

    connect(ui->searchText, &QLineEdit::returnPressed, this, &ModrinthModBrowser::search);
    connect(VersionManager::manager(), &VersionManager::modrinthVersionListUpdated, this, &ModrinthModBrowser::updateVersionList);

    if(localMod_){
        ui->searchText->setText(localMod_->commonInfo()->id());
    } else if(Config().getSearchModsOnStartup()){
        inited_ = true;
        search();
    }
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
}

void ModrinthModBrowser::load()
{
    if(!inited_){
        inited_ = true;
        search();
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
    search();
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
            search();
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
        search();
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
            search();
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
            search();
        }
    });
    ui->menuSelect_Category->addSeparator();
    auto anyCategoryAction = ui->menuSelect_Category->addAction(tr("Any"));
    connect(anyCategoryAction, &QAction::triggered, this, [=]{
        currentCategoryIds_.clear();
        ui->menuSelect_Category->setTitle(tr("Category : %1").arg(tr("Any")));
        ui->menuSelect_Category->setIcon(QIcon());
        search();
    });
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
            search();
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
    if(!refreshAction_->isEnabled()) return;
    manager_->search(ui->searchText->text(),
                     currentGameVersions_,
                     ModLoaderType::modrinth.at(ui->loaderSelect->currentIndex()),
                     currentCategoryIds_,
                     ui->sortSelect->currentIndex());
}

QDialog *ModrinthModBrowser::getDialog(const QModelIndex &index)
{
    auto mod = index.siblingAtColumn(ModrinthManagerModel::ModColumn).data(Qt::UserRole + 1).value<ModrinthMod*>();
    if(mod && mod->parent() == manager_){
        auto dialog = new ModrinthModDialog(this, mod);
        //set parent
        mod->setParent(dialog);
        connect(dialog, &ModrinthModDialog::finished, this, [=]{
            if(manager_->mods().contains(mod))
                mod->setParent(nullptr);
        });
        return dialog;
    }
    return nullptr;
}

void ModrinthModBrowser::on_sortSelect_currentIndexChanged(int)
{
    search();
}

void ModrinthModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    search();
}

void ModrinthModBrowser::onSelectedItemChanged(const QModelIndex &index)
{
    if(index.isValid())
        selectedMod_ = index.data(Qt::UserRole + 1).value<ModrinthMod*>();
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

QWidget *ModrinthModBrowser::getListViewIndexWidget(const QModelIndex &index)
{
    auto mod = index.data(Qt::UserRole + 1).value<ModrinthMod*>();
    if(mod){
        auto widget = new ModrinthModItemWidget(nullptr, mod);
        manager_->model()->setItemHeight(widget->height());
        return widget;
    } else
        return nullptr;
}

QWidget *ModrinthModBrowser::getTreeViewIndexWidget(const QModelIndex &index)
{
    auto mod = index.siblingAtColumn(ModrinthManagerModel::ModColumn).data(Qt::UserRole + 1).value<ModrinthMod*>();
    if(mod){
        auto tagsWidget = new TagsWidget(this);
        tagsWidget->setMod(mod);
        return tagsWidget;
    } else
        return nullptr;
}

void ModrinthModBrowser::loadMore()
{
    if(refreshAction_->isEnabled())
        manager_->searchMore();
}

ModrinthMod *ModrinthModBrowser::selectedMod() const
{
    return selectedMod_;
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
    if(selectedMod_ && selectedMod_->parent() == manager_){
        auto dialog = new ModrinthModDialog(this, selectedMod_);
        //set parent
        selectedMod_->setParent(dialog);
        connect(dialog, &ModrinthModDialog::finished, this, [=, mod = selectedMod_]{
            if(manager_->mods().contains(mod))
                selectedMod_->setParent(nullptr);
        });
        dialog->show();
    }
}

QMenu *ModrinthModBrowser::getCustomContextMenu()
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

ModrinthManager *ModrinthModBrowser::manager() const
{
    return manager_;
}
