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
#include <QLabel>

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
#include "download/qaria2.h"
#include "gameversion.h"
#include "config.hpp"
#include "util/funcutil.h"

ModManager::ModManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModManager),
    browserSelector_(new BrowserSelectorWidget(this))
{
    ui->setupUi(this);
    ui->actionAbout_Qt->setIcon(QIcon(":/qt-project.org/qmessagebox/images/qtlogo-64.png"));
    browserSelector_->setModel(ui->pageSwitcher->model());
    connect(browserSelector_, &BrowserSelectorWidget::browserChanged, ui->pageSwitcher, &PageSwitcher::setPage);
    connect(browserSelector_, &BrowserSelectorWidget::customContextMenuRequested, this, &ModManager::customContextMenuRequested);
    LocalModPathManager::load();

    Config config;
    resize(config.getMainWindowWidth(),
           config.getMainWindowHeight());
//    addToolBar(static_cast<Qt::ToolBarArea>(config.getTabSelectBarArea()), ui->toolBar);

    setStyleSheet("QListWidget::item:hover {"
                  "  border-left: 5px solid #eee;"
                  "}"
                  "QListWidget::item:selected {"
                  "  border-left: 5px solid #37d;"
                  "  color: black;"
                  "}");

    ui->pageSelectorDock->setWidget(browserSelector_);
    ui->pageSelectorDock->setTitleBarWidget(new QLabel);
    connect(ui->pageSelectorDock, &QDockWidget::visibilityChanged, ui->actionPage_Selector, &QAction::setChecked);

    //Download
    ui->pageSwitcher->addDownloadPage();

    //Explore
    if(config.getShowCurseforge()) ui->pageSwitcher->addCurseforgePage();
    if(config.getShowModrinth()) ui->pageSwitcher->addModrinthPage();
    if(config.getShowOptiFine()) ui->pageSwitcher->addOptiFinePage();
    if(config.getShowReplayMod()) ui->pageSwitcher->addReplayModPage();

    //Local
    syncPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModManager::syncPathList);

    //default browser
    if(ui->pageSwitcher->exploreBrowsers().size())
        ui->pageSwitcher->setPage(PageSwitcher::Explore, 0);

    //init versions
    VersionManager::initVersionLists();

    ui->actionShow_Mod_Authors->setChecked(config.getShowModAuthors());
    ui->actionShow_Mod_Date_Time->setChecked(config.getShowModDateTime());
    ui->actionShow_Mod_Category->setChecked(config.getShowModCategory());
    ui->actionShow_Mod_Loader_Type->setChecked(config.getShowModLoaderType());
}

ModManager::~ModManager()
{
//    Config().setTabSelectBarArea(toolBarArea(ui->toolBar));
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
    LocalModBrowser *selectedBrowser = nullptr;
    if(ui->pageSwitcher->currentCategory() == PageSwitcher::Local)
        selectedBrowser = ui->pageSwitcher->localModBrowser(ui->pageSwitcher->currentIndex());

    auto oldCount = pathList_.size();
    for(const auto &path : LocalModPathManager::pathList()){
        if(auto i = pathList_.indexOf(path); i < 0){
            //not present, new one
            pathList_ << path;
            ui->pageSwitcher->addLocalPage(path);
        } else{
            //present, move position
            oldCount--;
            auto path = pathList_.takeAt(i);
            pathList_ << path;
            auto browser = ui->pageSwitcher->takeLocalModBrowser(i);
            ui->pageSwitcher->addLocalPage(browser);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        pathList_.removeAt(i);
        ui->pageSwitcher->removeLocalModBrowser(i);
    }

    //they should be same after sync
    assert(pathList_ == LocalModPathManager::pathList());

    //reset selected path
    if(!selectedBrowser){
        if(auto index = ui->pageSwitcher->localModBrowsers().indexOf(selectedBrowser); index >= 0)
            ui->pageSwitcher->setPage(PageSwitcher::Local, index);
    }
}

void ModManager::editLocalPath(int index)
{
    auto pathInfo = LocalModPathManager::pathList().at(index)->info();
    auto dialog = new LocalModPathSettingsDialog(this, pathInfo);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo, bool autoLoaderType){
        LocalModPathManager::pathList().at(index)->setInfo(newInfo, autoLoaderType);
//        browserSelector_->localItem()->child(index)->setText(0, newInfo.displayName());
    });
    dialog->exec();
}

void ModManager::on_actionPreferences_triggered()
{
    auto preferences = new Preferences(this);
    preferences->exec();
    ui->pageSwitcher->updateUi();
    QAria2::qaria2()->updateOptions();
}

void ModManager::on_actionManage_Browser_triggered()
{
    auto dialog = new BrowserManagerDialog(this);
    connect(dialog, &BrowserManagerDialog::accepted, this, &ModManager::syncPathList);
    dialog->show();
}

void ModManager::customContextMenuRequested(const QModelIndex &index, const QPoint &pos)
{
    auto menu = new QMenu(this);
    if(!index.isValid()){
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
    } else if(index.parent().row() == PageSwitcher::Explore){
        // on one of explore items
        auto exploreBrowser = ui->pageSwitcher->exploreBrowser(index.row());
        connect(menu->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh")), &QAction::triggered, this, [=]{
            exploreBrowser->refresh();
        });
        menu->addAction(exploreBrowser->visitWebsiteAction());
    } else if(index.parent().row() == PageSwitcher::Local){
        // on one of local items
        auto localBrowser = ui->pageSwitcher->localModBrowser(index.row());
        connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
            editLocalPath(index.row());
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
            LocalModPathManager::removePathAt(index.row());
        });
        menu->addSeparator();
        menu->addAction(ui->actionManage_Browser);
    }
    if(!menu->actions().isEmpty())
        menu->exec(pos);
}

void ModManager::on_action_About_Mod_Manager_triggered()
{
    auto dialog = new AboutDialog(this);
    dialog->exec();
}

void ModManager::on_actionPage_Selector_toggled(bool arg1)
{
    ui->pageSelectorDock->setVisible(arg1);
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
    for(auto &&action : ui->menu_Help->actions()){
        if(action->data().toBool())
            ui->menu_Help->removeAction(action);
    }
    int count = 0;
    for(auto browser : ui->pageSwitcher->exploreBrowsers()){
        auto action = browser->visitWebsiteAction();
        action->setData(true);
        ui->menu_Help->insertAction(ui->menu_Help->actions().at(count++), action);
    }
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

void ModManager::on_actionShow_Mod_Authors_toggled(bool arg1)
{
    Config().setShowModAuthors(arg1);
    ui->pageSwitcher->updateUi();
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

void ModManager::on_actionNext_Page_triggered()
{
    ui->pageSwitcher->nextPage();
}

void ModManager::on_actionPrevious_Page_triggered()
{
    ui->pageSwitcher->previesPage();
}


void ModManager::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

