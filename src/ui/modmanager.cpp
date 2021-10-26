#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>
#include <QResizeEvent>
#include <QFileDialog>

#include "ui/aboutdialog.h"
#include "ui/browserselectorwidget.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "ui/local/localmodbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/download/downloadbrowser.h"
#include "ui/preferences.h"
#include "ui/browsermanagerdialog.h"
#include "ui/local/localmodpathsettingsdialog.h"
#include "gameversion.h"
#include "config.hpp"
#include "util/funcutil.h"

ModManager::ModManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModManager),
    browserSelector_(new BrowserSelectorWidget(this))
{
    ui->setupUi(this);
    LocalModPathManager::load();

    Config config;
    resize(config.getMainWindowWidth(),
           config.getMainWindowHeight());
    addToolBar(static_cast<Qt::ToolBarArea>(config.getTabSelectBarArea()), ui->toolBar);

    setStyleSheet("QListWidget::item:hover {"
                  "  border-left: 5px solid #eee;"
                  "}"
                  "QListWidget::item:selected {"
                  "  border-left: 5px solid #37d;"
                  "  color: black;"
                  "}");

    ui->toolBar->addWidget(browserSelector_);
    connect(browserSelector_, &BrowserSelectorWidget::browserChanged, ui->pageSwitcher, &PageSwitcher::setPage);
    connect(browserSelector_, &BrowserSelectorWidget::customContextMenuRequested, this, &ModManager::customContextMenuRequested);
    connect(ui->toolBar, &QToolBar::visibilityChanged, ui->action_Browsers, &QAction::setChecked);

    //Downloader
    ui->pageSwitcher->addDownloadPage();
    auto downloaderItem = new QTreeWidgetItem(browserSelector_->downloadItem(), {tr("Downloader")});
    browserSelector_->downloadItem()->addChild(downloaderItem);
    downloaderItem->setIcon(0, QIcon::fromTheme("download"));

    //Curseforge
    if(config.getShowCurseforge()){
        ui->pageSwitcher->addCurseforgePage();
        auto curseforgeItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"Curseforge"});
        browserSelector_->exploreItem()->addChild(curseforgeItem);
        curseforgeItem->setIcon(0, QIcon(":/image/curseforge.svg"));
    }

    //Modrinth
    if(config.getShowModrinth()){
        ui->pageSwitcher->addModrinthPage();
        auto modrinthItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"Modrinth"});
        browserSelector_->exploreItem()->addChild(modrinthItem);
        modrinthItem->setIcon(0, QIcon(":/image/modrinth.svg"));
    }

    //Optifine
    if(config.getShowOptiFine()){
        ui->pageSwitcher->addOptiFinePage();
        auto optifineItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"OptiFine"});
        browserSelector_->exploreItem()->addChild(optifineItem);
        optifineItem->setIcon(0, QIcon(":/image/optifine.png"));
    }

    //Replay
    if(config.getShowReplayMod()){
        ui->pageSwitcher->addReplayModPage();
        auto replayItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"ReplayMod"});
        browserSelector_->exploreItem()->addChild(replayItem);
        replayItem->setIcon(0, QIcon(":/image/replay.png"));
    }

    //Local
    syncPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModManager::syncPathList);

    //default browser
//    browserSelector_->browserTreeWidget()->setCurrentItem(curseforgeItem);
    if(ui->pageSwitcher->exploreBrowsers().size())
        ui->pageSwitcher->setPage(PageSwitcher::Explore, 0);

    //init versions
    VersionManager::initVersionLists();

    ui->actionShow_Mod_Date_Time->setChecked(config.getShowModDateTime());
    ui->actionShow_Mod_Category->setChecked(config.getShowModCategory());
    ui->actionShow_Mod_Loader_Type->setChecked(config.getShowModLoaderType());
}

ModManager::~ModManager()
{
    Config().setTabSelectBarArea(toolBarArea(ui->toolBar));
    delete ui;
}

void ModManager::resizeEvent(QResizeEvent *event)
{
    Config config;
    config.setMainWindowWidth(event->size().width());
    config.setMainWindowHeight(event->size().height());
}

void ModManager::syncPathList()
{
    //remember selected path
    LocalModPath *selectedPath = nullptr;
    auto currentItem = browserSelector_->browserTreeWidget()->currentItem();
    if(currentItem != nullptr && currentItem->parent() == browserSelector_->localItem()){
        auto index = currentItem->parent()->indexOfChild(currentItem);
        selectedPath = pathList_.at(index);
    }

    auto oldCount = pathList_.size();
    for(const auto &path : LocalModPathManager::pathList()){
        if(auto i = pathList_.indexOf(path); i < 0){
            //not present, new one
            pathList_ << path;
            auto item = new QTreeWidgetItem(browserSelector_->localItem(), { path->info().displayName() });
            browserSelector_->localItem()->addChild(item);
            if(auto loaderType = path->info().loaderType(); loaderType == ModLoaderType::Any)
                item->setIcon(0, QIcon::fromTheme("folder"));
            else
                item->setIcon(0, ModLoaderType::icon(path->info().loaderType()));
            ui->pageSwitcher->addLocalPage(path);
        } else{
            //present, move position
            oldCount--;
            auto path = pathList_.takeAt(i);
            pathList_ << path;
            auto item = browserSelector_->localItem()->takeChild(i);
            item->setText(0, path->info().displayName());
            if(auto loaderType = path->info().loaderType(); loaderType == ModLoaderType::Any)
                item->setIcon(0, QIcon::fromTheme("folder"));
            else
                item->setIcon(0, ModLoaderType::icon(path->info().loaderType()));
            browserSelector_->localItem()->addChild(item);
            auto browser = ui->pageSwitcher->takeLocalModBrowser(i);
            ui->pageSwitcher->addLocalPage(browser);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        pathList_.removeAt(i);
        delete browserSelector_->localItem()->takeChild(i);
        ui->pageSwitcher->removeLocalModBrowser(i);
    }

    //they should be same after sync
    assert(pathList_ == LocalModPathManager::pathList());

    //reset selected path
    if(selectedPath != nullptr){
        auto index = pathList_.indexOf(selectedPath);
        if(index >= 0)
            browserSelector_->browserTreeWidget()->setCurrentItem(browserSelector_->localItem()->child(index));
    }
}

void ModManager::editLocalPath(int index)
{
    auto pathInfo = LocalModPathManager::pathList().at(index)->info();
    auto dialog = new LocalModPathSettingsDialog(this, pathInfo);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo, bool autoLoaderType){
        LocalModPathManager::pathList().at(index)->setInfo(newInfo, autoLoaderType);
        browserSelector_->localItem()->child(index)->setText(0, newInfo.displayName());
    });
    dialog->exec();
}

void ModManager::on_actionPreferences_triggered()
{
    auto preferences = new Preferences(this);
    preferences->exec();
}

void ModManager::on_actionManage_Browser_triggered()
{
    auto dialog = new BrowserManagerDialog(this);
    connect(dialog, &BrowserManagerDialog::accepted, this, &ModManager::syncPathList);
    dialog->show();
}

void ModManager::customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    auto item = browserSelector_->browserTreeWidget()->itemAt(pos);
    if(item == nullptr){
        // in empty area
        connect(menu->addAction(QIcon::fromTheme("list-add"), tr("New Mod Path")), &QAction::triggered, this, [=]{
            auto dialog = new LocalModPathSettingsDialog(this);
            dialog->show();
            connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
                auto path = new LocalModPath(pathInfo, autoLoaderType);
                LocalModPathManager::addPath(path);
            });
        });
        menu->addSeparator();
        menu->addAction(ui->actionManage_Browser);
    } else if(item->parent() == browserSelector_->exploreItem()){
        // on one of explore items
        auto index = item->parent()->indexOfChild(item);
        auto exploreBrowser = ui->pageSwitcher->exploreBrowser(index);
        connect(menu->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh")), &QAction::triggered, this, [=]{
            exploreBrowser->refresh();
        });
        menu->addAction(exploreBrowser->visitWebsiteAction());
    }else if(item->parent() == browserSelector_->localItem()){
        // on one of local items
        auto index = item->parent()->indexOfChild(item);
        auto localBrowser = ui->pageSwitcher->localModBrowser(index);
        connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
            editLocalPath(index);
        });
        auto reloadAction = menu->addAction(QIcon::fromTheme("view-refresh"), tr("Reload"));
        if(localBrowser->isLoading()){
            reloadAction->setEnabled(false);
            connect(localBrowser, &LocalModBrowser::loadFinished, this, [=]{
                reloadAction->setEnabled(true);
            });
        }
        connect(reloadAction, &QAction::triggered, this, [=]{
            localBrowser->reload();
        });
        connect(menu->addAction(QIcon::fromTheme("delete"), tr("Delete")), &QAction::triggered, this, [=]{
            if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete this mod path?"))) return;
            LocalModPathManager::removePathAt(index);
        });
        menu->addSeparator();
        menu->addAction(ui->actionManage_Browser);
    }
    if(!menu->actions().isEmpty())
        menu->exec(browserSelector_->browserTreeWidget()->mapToGlobal(pos));
}

void ModManager::on_action_About_Mod_Manager_triggered()
{
    auto dialog = new AboutDialog(this);
    dialog->exec();
}

void ModManager::on_action_Browsers_toggled(bool arg1)
{
    ui->toolBar->setVisible(arg1);
}

void ModManager::on_actionOpen_new_path_dialog_triggered()
{
    auto dialog = new LocalModPathSettingsDialog(this);
    dialog->exec();
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
        auto path = new LocalModPath(pathInfo, autoLoaderType);
        LocalModPathManager::addPath(path);
    });
}

void ModManager::on_actionSelect_A_Directory_triggered()
{
    auto pathStr = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), Config().getCommonPath());
    if(pathStr.isEmpty()) return;
    auto path = new LocalModPath(LocalModPathInfo::deduceFromPath(pathStr), true);
    LocalModPathManager::addPath(path);
}

void ModManager::on_actionSelect_Multiple_Directories_triggered()
{
    auto paths = getExistingDirectories(this, tr("Select your mod directories..."), Config().getCommonPath());
    LocalModPathManager::addPaths(paths);
}

void ModManager::on_menu_Path_aboutToShow()
{
    if(ui->pageSwitcher->currentCategory() == PageSwitcher::Download)
        ui->actionReload->setEnabled(false);
    if(ui->pageSwitcher->currentCategory() == PageSwitcher::Local)
        if(auto localBrowser = ui->pageSwitcher->localModBrowser(ui->pageSwitcher->currentPage()); localBrowser && localBrowser->isLoading()){
            ui->actionReload->setEnabled(false);
            connect(localBrowser, &LocalModBrowser::loadFinished, this, [=]{
                ui->actionReload->setEnabled(true);
            });
        }
}

void ModManager::on_menu_Help_aboutToShow()
{
    ui->menu_Help->clear();
    for(auto browser : ui->pageSwitcher->exploreBrowsers())
        ui->menu_Help->addAction(browser->visitWebsiteAction());
    ui->menu_Help->addSeparator();
    ui->menu_Help->addAction(ui->action_About_Mod_Manager);
}

void ModManager::on_menuPaths_aboutToShow()
{
    ui->menuPaths->clear();
    int index = 0;
    for(auto path : LocalModPathManager::pathList()){
        auto action = new QAction(path->info().displayName());
        connect(action, &QAction::triggered, this, [=]{
            ui->pageSwitcher->setPage(PageSwitcher::Local, index);
        });
        ui->menuPaths->addAction(action);
        index++;
    }
}

void ModManager::on_menuTags_aboutToShow()
{
    auto showTagCategories = Config().getShowTagCategories();
    ui->menuTags->clear();
    int count = 0;
    for(const auto &category : TagCategory::PresetCategories){
        auto action = new QAction(category.name());
        action->setCheckable(true);
        if(showTagCategories.contains(category)){
            action->setChecked(true);
            count++;
        }
        connect(action, &QAction::toggled, this, [=](bool bl){
            auto list = showTagCategories;
            if(bl)
                list.insert(count, category) ;
            else
                list.removeAll(category);
            Config().setShowTagCategories(list);
            ui->pageSwitcher->updateUi();
        });
        ui->menuTags->addAction(action);
    }
}

void ModManager::on_actionReload_triggered()
{
    if(auto category = ui->pageSwitcher->currentCategory(); category == PageSwitcher::Explore)
        ui->pageSwitcher->exploreBrowser(ui->pageSwitcher->currentPage())->refresh();
    else if(category == PageSwitcher::Local)
        ui->pageSwitcher->localModBrowser(ui->pageSwitcher->currentPage())->reload();
}


void ModManager::on_actionShow_Mod_Date_Time_toggled(bool arg1)
{
    Config().setShowModDateTime(arg1);
    ui->pageSwitcher->updateUi();
}


void ModManager::on_actionShow_Mod_Category_toggled(bool arg1)
{
    Config().setShowModCategory(arg1);
    ui->pageSwitcher->updateUi();
}


void ModManager::on_actionShow_Mod_Loader_Type_toggled(bool arg1)
{
    Config().setShowModLoaderType(arg1);
    ui->pageSwitcher->updateUi();
}

