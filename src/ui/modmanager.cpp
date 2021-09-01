#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "localmodbrowser.h"
#include "curseforgemodbrowser.h"
#include "modrinthmodbrowser.h"
#include "preferences.h"
#include "browsermanagerdialog.h"
#include "localmodbrowsersettingsdialog.h"
#include "gameversion.h"
#include "config.h"

ModManager::ModManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModManager),
    updateVersionsWatcher(new QFutureWatcher<void>(this))
{
    ui->setupUi(this);
    for(int i = ui->stackedWidget->count(); i >= 0; i--)
    {
        QWidget* widget = ui->stackedWidget->widget(i);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }

    //load mod dirs in config
    Config config;
    for(const auto &variant : config.getDirList())
        modDirList << ModDirInfo::fromVariant(variant);

    //Curseforge
    auto curseforgeModBrowser = new CurseforgeModBrowser(this);
    auto curseforgeItem = new QListWidgetItem(QIcon(":/image/curseforge.svg"), "Curseforge");
    ui->modDirSelectorWidget->addItem(curseforgeItem);
    ui->stackedWidget->addWidget(curseforgeModBrowser);

    //Modrinth
    auto modrinthModBrowser = new ModrinthModBrowser(this);
    auto modrinthItem = new QListWidgetItem(QIcon(":/image/modrinth.svg"), "Modrinth");
    ui->modDirSelectorWidget->addItem(modrinthItem);
    ui->stackedWidget->addWidget(modrinthModBrowser);

    //Local
    for(const auto &modDirInfo : qAsConst(modDirList)){
        if(modDirInfo.exists()) {
            auto item = new QListWidgetItem(modDirInfo.showText());
            auto localModBrowser = new LocalModBrowser(this, modDirInfo);
            dirWidgetItemList.append(item);
            localModBrowserList.append(localModBrowser);
            ui->modDirSelectorWidget->addItem(item);
            ui->stackedWidget->addWidget(localModBrowser);
        }
    }

    //check and update version
    QFuture<void> future = QtConcurrent::run(&GameVersion::initVersionList);
    updateVersionsWatcher->setFuture(future);
    connect(updateVersionsWatcher, &QFutureWatcher<void>::finished, curseforgeModBrowser, &CurseforgeModBrowser::updateVersions);
}

ModManager::~ModManager()
{
    delete ui;
}

void ModManager::refreshBrowsers()
{
    //load mod dirs in config
    Config config;
    auto oldCount = modDirList.size();
    for(const auto &variant : config.getDirList()){
        auto modDirInfo = ModDirInfo::fromVariant(variant);
        if(!modDirInfo.exists()) continue;
        auto i = modDirList.indexOf(modDirInfo);
        if(i < 0){
            //not present, new one
            modDirList << modDirInfo;
            auto item = new QListWidgetItem(modDirInfo.showText());
            auto localModBrowser = new LocalModBrowser(this, modDirInfo);
            dirWidgetItemList.append(item);
            localModBrowserList.append(localModBrowser);
            ui->modDirSelectorWidget->addItem(item);
            ui->stackedWidget->addWidget(localModBrowser);
        } else{
            //present, move position
            oldCount--;
            auto j = i + 2;
            modDirList << modDirList.takeAt(i);
            dirWidgetItemList << dirWidgetItemList.takeAt(i);
            localModBrowserList << localModBrowserList.takeAt(i);
            ui->modDirSelectorWidget->addItem(ui->modDirSelectorWidget->takeItem(j));
            auto widget = ui->stackedWidget->widget(j);
            ui->stackedWidget->removeWidget(widget);
            ui->stackedWidget->addWidget(widget);
        }
    }
    //remove remained mod dir
    auto i = oldCount;
    while (i--) {
        auto j = i + 2;
        modDirList.removeAt(i);
        dirWidgetItemList.removeAt(i);
        localModBrowserList.at(i);
        delete ui->modDirSelectorWidget->takeItem(j);
        auto widget = ui->stackedWidget->widget(j);
        ui->stackedWidget->removeWidget(widget);
        //TODO: remove it improperly will cause program to crash
//        widget->deleteLater();
    }

}

void ModManager::on_modDirSelectorWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void ModManager::on_newLocalBrowserButton_clicked()
{
    auto dialog = new LocalModBrowserSettingsDialog(this);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &modDirInfo){
        modDirList << modDirInfo;
        auto item = new QListWidgetItem(modDirInfo.showText());
        auto localModBrowser = new LocalModBrowser(this, modDirInfo);
        dirWidgetItemList.append(item);
        localModBrowserList.append(localModBrowser);
        ui->modDirSelectorWidget->addItem(item);
        ui->stackedWidget->addWidget(localModBrowser);

        Config config;
        QList<QVariant> list;
        for(const auto &dirInfo : qAsConst(modDirList))
            list << dirInfo.toVariant();
        config.setDirList(list);
    });
    connect(updateVersionsWatcher, &QFutureWatcher<void>::finished, dialog, &LocalModBrowserSettingsDialog::updateVersions);
    dialog->exec();
}


void ModManager::on_modDirSelectorWidget_doubleClicked(const QModelIndex &index)
{
    auto row = index.row();
    //exclude curseforge and modrinth page
    if(row <= 1) return;

    auto modDirInfo = modDirList.at(row - 2);
    auto dialog = new LocalModBrowserSettingsDialog(this, modDirInfo);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &newInfo){
        //exclude curseforge and modrinth page
        modDirList[row - 2] = newInfo;
        dirWidgetItemList[row - 2]->setText(newInfo.showText());
        localModBrowserList[row - 2]->setModDirInfo(newInfo);
    });
    connect(updateVersionsWatcher, &QFutureWatcher<void>::finished, dialog, &LocalModBrowserSettingsDialog::updateVersions);
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
    connect(dialog, &BrowserManagerDialog::accepted, this, &ModManager::refreshBrowsers);
    dialog->show();
}

