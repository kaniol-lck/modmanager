#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>

#include "ui/aboutdialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "ui/local/localmodbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/downloadbrowser.h"
#include "ui/preferences.h"
#include "ui/browsermanagerdialog.h"
#include "ui/local/localmodpathsettingsdialog.h"
#include "gameversion.h"
#include "config.h"

ModManager::ModManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModManager),
    downloadItem_(new QTreeWidgetItem({tr("Download")})),
    exploreItem_(new QTreeWidgetItem({tr("Explore")})),
    localItem_(new QTreeWidgetItem({tr("Local")}))
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 3);

    //setup tree widget
    for (const auto &item : {downloadItem_, exploreItem_, localItem_}){
        item->setForeground(0, Qt::gray);
        item->setFlags(item->flags().setFlag(Qt::ItemIsSelectable, false));
        ui->browserTreeWidget->addTopLevelItem(item);
    }
    ui->browserTreeWidget->expandAll();

    //Downloader
    auto downloadBrowser = new DownloadBrowser(this);
    auto downloaderItem = new QTreeWidgetItem(downloadItem_, {tr("Downloader")});
    downloadItem_->addChild(downloaderItem);
    downloaderItem->setIcon(0, QIcon::fromTheme("download"));
    ui->stackedWidget->addWidget(downloadBrowser);

    //Curseforge
    curseforgeModBrowser_ = new CurseforgeModBrowser(this);
    auto curseforgeItem = new QTreeWidgetItem(exploreItem_, {"Curseforge"});
    exploreItem_->addChild(curseforgeItem);
    curseforgeItem->setIcon(0, QIcon(":/image/curseforge.svg"));
    ui->stackedWidget->addWidget(curseforgeModBrowser_);

    //Modrinth
    modrinthModBrowser_ = new ModrinthModBrowser(this);
    auto modrinthItem = new QTreeWidgetItem(exploreItem_, {"Modrinth"});
    exploreItem_->addChild(modrinthItem);
    modrinthItem->setIcon(0, QIcon(":/image/modrinth.svg"));
    ui->stackedWidget->addWidget(modrinthModBrowser_);

    //Optifine
    optifineModBrowser_ = new OptifineModBrowser(this);
    auto optifineItem = new QTreeWidgetItem(exploreItem_, {"OptiFine"});
    exploreItem_->addChild(optifineItem);
    optifineItem->setIcon(0, QIcon(":/image/optifine.png"));
    ui->stackedWidget->addWidget(optifineModBrowser_);

    //Local
    syncPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModManager::syncPathList);

    //default browser
    ui->browserTreeWidget->setCurrentItem(curseforgeItem);

    //init versions
    VersionManager::initVersionLists();
}

ModManager::~ModManager()
{
    delete ui;
}

void ModManager::syncPathList()
{
    //remember selected path
    LocalModPath *selectedPath = nullptr;
    auto currentItem = ui->browserTreeWidget->currentItem();
    if(currentItem != nullptr && currentItem->parent() == localItem_){
        auto index = currentItem->parent()->indexOfChild(currentItem);
        selectedPath = pathList_.at(index);
    }

    auto oldCount = pathList_.size();
    for(const auto &path : LocalModPathManager::pathList()){
        auto i = pathList_.indexOf(path);
        if(i < 0){
            //not present, new one
            pathList_ << path;
            auto item = new QTreeWidgetItem(localItem_, {path->info().displayName()});
            localItem_->addChild(item);
            if(path->info().loaderType() == ModLoaderType::Fabric)
                item->setIcon(0, QIcon(":/image/fabric.png"));
            else if(path->info().loaderType() == ModLoaderType::Forge)
                item->setIcon(0, QIcon(":/image/forge.svg"));
            else
                item->setIcon(0, QIcon::fromTheme("folder"));
            auto localModBrowser = new LocalModBrowser(this, path);
            ui->stackedWidget->addWidget(localModBrowser);

            connect(localModBrowser, &LocalModBrowser::findNewOnCurseforge, curseforgeModBrowser_, &CurseforgeModBrowser::searchModByPathInfo);
            connect(localModBrowser, &LocalModBrowser::findNewOnCurseforge, this, [=]{
                ui->browserTreeWidget->setCurrentItem(exploreItem_->child(0));
            });

            connect(localModBrowser, &LocalModBrowser::findNewOnModrinth, modrinthModBrowser_, &ModrinthModBrowser::searchModByPathInfo);
            connect(localModBrowser, &LocalModBrowser::findNewOnModrinth, this, [=]{
                ui->browserTreeWidget->setCurrentItem(exploreItem_->child(1));
            });
        } else{
            //present, move position
            oldCount--;
            pathList_ << pathList_.takeAt(i);
            localItem_->addChild(localItem_->takeChild(i));
            //TODO: magic number
            auto widget = ui->stackedWidget->widget(i + 4);
            ui->stackedWidget->removeWidget(widget);
            ui->stackedWidget->addWidget(widget);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        //TODO: magic number
        auto j = i + 4;
        pathList_.removeAt(i);
        delete localItem_->takeChild(i);
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
            ui->browserTreeWidget->setCurrentItem(localItem_->child(index));
    }
}

void ModManager::editLocalPath(int index)
{
    auto pathInfo = LocalModPathManager::pathList().at(index)->info();
    auto dialog = new LocalModPathSettingsDialog(this, pathInfo);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo){
        LocalModPathManager::pathList().at(index)->setInfo(newInfo);
        localItem_->child(index)->setText(0, newInfo.displayName());
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

void ModManager::on_browserTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{
    if(current == nullptr) return;
    auto parent = current->parent();

    if(parent == downloadItem_){
        ui->stackedWidget->setCurrentIndex(0);
    } else if(parent == exploreItem_){
        auto index = parent->indexOfChild(current);
        ui->stackedWidget->setCurrentIndex(1 + index);
    } else if(parent == localItem_){
        auto index = parent->indexOfChild(current);
        //TODO: magic number
        ui->stackedWidget->setCurrentIndex(4 + index);
    }
}

void ModManager::on_browserTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    if(item->parent() != localItem_) return;
    auto index = localItem_->indexOfChild(item);
    editLocalPath(index);
}

void ModManager::on_browserTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    auto item = ui->browserTreeWidget->itemAt(pos);
    if(item == nullptr){
        // in empty area
        connect(menu->addAction(QIcon::fromTheme("list-add"), tr("New Mod Path")), &QAction::triggered, this, [=]{
            auto dialog = new LocalModPathSettingsDialog(this);
            dialog->show();
            connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo){
                auto path = new LocalModPath(this, pathInfo);
                LocalModPathManager::addPath(path);
            });
        });
        menu->addSeparator();
        menu->addAction(ui->actionManage_Browser);
    } else if(item->parent() == localItem_){
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
        menu->exec(ui->browserTreeWidget->mapToGlobal(pos));
}

void ModManager::on_action_About_Mod_Manager_triggered()
{
    auto dialog = new AboutDialog(this);
    dialog->exec();
}


void ModManager::on_actionView_Curseforge_triggered()
{
    QUrl url("https://www.curseforge.com/minecraft/mc-mods");
    QDesktopServices::openUrl(url);
}


void ModManager::on_actionVisit_Modrinth_triggered()
{
    QUrl url("https://modrinth.com/mods");
    QDesktopServices::openUrl(url);
}

