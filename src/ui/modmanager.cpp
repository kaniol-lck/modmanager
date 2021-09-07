#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QMessageBox>

#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "localmodbrowser.h"
#include "curseforgemodbrowser.h"
#include "modrinthmodbrowser.h"
#include "downloadbrowser.h"
#include "preferences.h"
#include "browsermanagerdialog.h"
#include "localmodbrowsersettingsdialog.h"
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
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 2);

    //setup tree widget
    for (const auto &item : {downloadItem_, exploreItem_, localItem_}){
        item->setForeground(0, Qt::gray);
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
    auto curseforgeModBrowser = new CurseforgeModBrowser(this);
    auto curseforgeItem = new QTreeWidgetItem(exploreItem_, {tr("Curseforge")});
    exploreItem_->addChild(curseforgeItem);
    curseforgeItem->setIcon(0, QIcon(":/image/curseforge.svg"));
    ui->stackedWidget->addWidget(curseforgeModBrowser);

    //Modrinth
    auto modrinthModBrowser = new ModrinthModBrowser(this);
    auto modrinthItem = new QTreeWidgetItem(exploreItem_, {tr("Modrinth")});
    exploreItem_->addChild(modrinthItem);
    modrinthItem->setIcon(0, QIcon(":/image/modrinth.svg"));
    ui->stackedWidget->addWidget(modrinthModBrowser);

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
    auto oldCount = pathList_.size();
    for(const auto &path : LocalModPathManager::pathList()){
        auto i = pathList_.indexOf(path);
        if(i < 0){
            //not present, new one
            pathList_ << path;
            auto item = new QTreeWidgetItem(localItem_, {path->info().showText()});
            localItem_->addChild(item);
            item->setIcon(0, QIcon::fromTheme("folder"));
            auto localModBrowser = new LocalModBrowser(this, path);
            ui->stackedWidget->addWidget(localModBrowser);
        } else{
            //present, move position
            oldCount--;
            pathList_ << pathList_.takeAt(i);
            localItem_->addChild(localItem_->takeChild(i));
            auto widget = ui->stackedWidget->widget(i + 3);
            ui->stackedWidget->removeWidget(widget);
            ui->stackedWidget->addWidget(widget);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        auto j = i + 3;
        pathList_.removeAt(i);
        delete localItem_->takeChild(i);
        auto widget = ui->stackedWidget->widget(j);
        ui->stackedWidget->removeWidget(widget);
        //TODO: remove it improperly will cause program to crash
//        widget->deleteLater();
    }

    //they should be same after sync
    assert(pathList_ == LocalModPathManager::pathList());
}

void ModManager::editLocalPath(int index)
{
    auto pathInfo = LocalModPathManager::pathList().at(index)->info();
    auto dialog = new LocalModBrowserSettingsDialog(this, pathInfo);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo){
        LocalModPathManager::pathList().at(index)->setInfo(newInfo);
        localItem_->child(index)->setText(0, newInfo.showText());
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

void ModManager::on_browserTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current == nullptr) return;
    auto parent = current->parent();
    if(parent == nullptr){
        if(previous != nullptr)
            ui->browserTreeWidget->setCurrentItem(previous);
        return;
    } else if(parent == downloadItem_){
        ui->stackedWidget->setCurrentIndex(0);
    } else if(parent == exploreItem_){
        auto index = parent->indexOfChild(current);
        ui->stackedWidget->setCurrentIndex(1 + index);
    } else if(parent == localItem_){
        auto index = parent->indexOfChild(current);
        ui->stackedWidget->setCurrentIndex(3 + index);
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
            auto dialog = new LocalModBrowserSettingsDialog(this);
            dialog->show();
            connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo){
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
