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

#include "ui/aboutdialog.h"
#include "ui/browserselectorwidget.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "ui/local/localmodbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/downloadbrowser.h"
#include "ui/preferences.h"
#include "ui/browsermanagerdialog.h"
#include "ui/local/localmodpathsettingsdialog.h"
#include "gameversion.h"
#include "config.hpp"

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
    connect(browserSelector_, &BrowserSelectorWidget::browserChanged, this, &ModManager::browserChanged);
    connect(browserSelector_, &BrowserSelectorWidget::customContextMenuRequested, this, &ModManager::customContextMenuRequested);

    //Downloader
    auto downloadBrowser = new DownloadBrowser(this);
    auto downloaderItem = new QTreeWidgetItem(browserSelector_->downloadItem(), {tr("Downloader")});
    browserSelector_->downloadItem()->addChild(downloaderItem);
    downloaderItem->setIcon(0, QIcon::fromTheme("download"));
    ui->stackedWidget->addWidget(downloadBrowser);

    //Curseforge
    curseforgeModBrowser_ = new CurseforgeModBrowser(this);
    auto curseforgeItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"Curseforge"});
    browserSelector_->exploreItem()->addChild(curseforgeItem);
    curseforgeItem->setIcon(0, QIcon(":/image/curseforge.svg"));
    ui->stackedWidget->addWidget(curseforgeModBrowser_);

    //Modrinth
    modrinthModBrowser_ = new ModrinthModBrowser(this);
    auto modrinthItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"Modrinth"});
    browserSelector_->exploreItem()->addChild(modrinthItem);
    modrinthItem->setIcon(0, QIcon(":/image/modrinth.svg"));
    ui->stackedWidget->addWidget(modrinthModBrowser_);

    //Optifine
    optifineModBrowser_ = new OptifineModBrowser(this);
    auto optifineItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"OptiFine"});
    browserSelector_->exploreItem()->addChild(optifineItem);
    optifineItem->setIcon(0, QIcon(":/image/optifine.png"));
    ui->stackedWidget->addWidget(optifineModBrowser_);

    //Replay
    replayModBrowser_ = new ReplayModBrowser(this);
    auto replayItem = new QTreeWidgetItem(browserSelector_->exploreItem(), {"ReplayMod"});
    browserSelector_->exploreItem()->addChild(replayItem);
    replayItem->setIcon(0, QIcon(":/image/replay.png"));
    ui->stackedWidget->addWidget(replayModBrowser_);

    //Local
    syncPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModManager::syncPathList);

    //default browser
    browserSelector_->browserTreeWidget()->setCurrentItem(curseforgeItem);

    //init versions
    VersionManager::initVersionLists();
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

void ModManager::browserChanged(BrowserSelectorWidget::BrowserCategory category, int index)
{
    int pageIndex = 0;
    using bsw = BrowserSelectorWidget;
    for(auto &&c : { bsw::Download, bsw::Explore, bsw::Local}){
        if(category == c){
            pageIndex += index;
            break;
        } else
            pageIndex += browserSelector_->item(c)->childCount();
    }
    ui->stackedWidget->setCurrentIndex(pageIndex);
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
            auto localModBrowser = new LocalModBrowser(this, path);
            ui->stackedWidget->addWidget(localModBrowser);

            connect(localModBrowser, &LocalModBrowser::findNewOnCurseforge, curseforgeModBrowser_, &CurseforgeModBrowser::searchModByPathInfo);
            connect(localModBrowser, &LocalModBrowser::findNewOnCurseforge, this, [=]{
                browserSelector_->browserTreeWidget()->setCurrentItem(browserSelector_->exploreItem()->child(0));
            });
            connect(localModBrowser, &LocalModBrowser::findNewOnModrinth, modrinthModBrowser_, &ModrinthModBrowser::searchModByPathInfo);
            connect(localModBrowser, &LocalModBrowser::findNewOnModrinth, this, [=]{
                browserSelector_->browserTreeWidget()->setCurrentItem(browserSelector_->exploreItem()->child(1));
            });
            connect(localModBrowser, &LocalModBrowser::findNewOnOptifine, optifineModBrowser_, &OptifineModBrowser::searchModByPathInfo);
            connect(localModBrowser, &LocalModBrowser::findNewOnOptifine, this, [=]{
                browserSelector_->browserTreeWidget()->setCurrentItem(browserSelector_->exploreItem()->child(2));
            });
            connect(localModBrowser, &LocalModBrowser::findNewOnReplay, replayModBrowser_, &ReplayModBrowser::searchModByPathInfo);
            connect(localModBrowser, &LocalModBrowser::findNewOnReplay, this, [=]{
                browserSelector_->browserTreeWidget()->setCurrentItem(browserSelector_->exploreItem()->child(3));
            });
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
            //TODO: magic number
            auto widget = ui->stackedWidget->widget(i + 5);
            ui->stackedWidget->removeWidget(widget);
            ui->stackedWidget->addWidget(widget);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        //TODO: magic number
        auto j = i + 5;
        pathList_.removeAt(i);
        delete browserSelector_->localItem()->takeChild(i);
        auto widget = ui->stackedWidget->widget(j);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
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
    preferences->show();
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
        connect(menu->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh")), &QAction::triggered, this, [=]{
            if(index == 0)
                curseforgeModBrowser_->refresh();
            if(index == 1)
                modrinthModBrowser_->refresh();
            if(index == 2)
                optifineModBrowser_->refresh();
            if(index == 3)
                replayModBrowser_->refresh();
        });
        if(index == 0)
            menu->addAction(ui->actionVisit_Curseforge);
        if(index == 1)
            menu->addAction(ui->actionVisit_Modrinth);
        if(index == 2)
            menu->addAction(ui->actionVisit_OptiFine);
        if(index == 3)
            menu->addAction(ui->actionVisit_ReplayMod);
    }else if(item->parent() == browserSelector_->localItem()){
        // on one of local items
        auto index = item->parent()->indexOfChild(item);

        connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
            editLocalPath(index);
        });
        connect(menu->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh")), &QAction::triggered, this, [=]{
            LocalModPathManager::pathList().at(index)->loadMods();
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


void ModManager::on_actionVisit_Curseforge_triggered()
{
    QUrl url("https://www.curseforge.com/minecraft/mc-mods");
    QDesktopServices::openUrl(url);
}

void ModManager::on_actionVisit_Modrinth_triggered()
{
    QUrl url("https://modrinth.com/mods");
    QDesktopServices::openUrl(url);
}

void ModManager::on_actionVisit_OptiFine_triggered()
{
    QUrl url("https://www.optifine.net");
    QDesktopServices::openUrl(url);
}

void ModManager::on_actionVisit_ReplayMod_triggered()
{
    QUrl url("https://www.replaymod.com");
    QDesktopServices::openUrl(url);
}
