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
#include "localmodbrowsersettingsdialog.h"
#include "gameversion.h"

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

    ModDirInfo modDirInfo(QDir("/run/media/kaniol/SanDisk/Minecraft/1.17.1/.minecraft/mods/"), GameVersion("1.17.1"), ModLoaderType::Fabric);
    ModDirInfo modDirInfo2(QDir("/run/media/kaniol/SanDisk/Minecraft/1.16.4/.minecraft/mods-test/"), GameVersion("1.16.4"), ModLoaderType::Fabric);

    modDirList.append(modDirInfo);
    modDirList.append(modDirInfo2);

    auto curseforgeModBrowser = new CurseforgeModBrowser(this);

    ui->modDirSelectorWidget->addItem("Curseforge");
    ui->stackedWidget->addWidget(curseforgeModBrowser);

    for(const auto &modDirInfo : qAsConst(modDirList)){
        if(modDirInfo.exists()) {
            auto item = new QListWidgetItem(modDirInfo.getGameVersion());
            auto localModBrowser = new LocalModBrowser(this, modDirInfo);
            dirWidgetItemList.append(item);
            localModBrowserList.append(localModBrowser);
            ui->modDirSelectorWidget->addItem(item);
            ui->stackedWidget->addWidget(localModBrowser);
        }
    }

    //start update
    QFuture<void> future = QtConcurrent::run(&GameVersion::initVersionList);
    updateVersionsWatcher->setFuture(future);
    connect(updateVersionsWatcher, &QFutureWatcher<void>::finished, curseforgeModBrowser, &CurseforgeModBrowser::updateVersions);
}

ModManager::~ModManager()
{
    delete ui;
}

void ModManager::on_modDirSelectorWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void ModManager::on_newLocalBrowserButton_clicked()
{
    auto dialog = new LocalModBrowserSettingsDialog(this);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &modDirInfo){
        modDirList.append(modDirInfo);
        auto item = new QListWidgetItem(modDirInfo.getGameVersion());
        auto localModBrowser = new LocalModBrowser(this, modDirInfo);
        dirWidgetItemList.append(item);
        localModBrowserList.append(localModBrowser);
        ui->modDirSelectorWidget->addItem(item);
        ui->stackedWidget->addWidget(localModBrowser);
        qDebug()<< modDirInfo.getModDir();
    });
    connect(updateVersionsWatcher, &QFutureWatcher<void>::finished, dialog, &LocalModBrowserSettingsDialog::updateVersions);
    dialog->exec();
}


void ModManager::on_modDirSelectorWidget_doubleClicked(const QModelIndex &index)
{
    auto row = index.row();
    //exclude curseforge page
    if(row <= 0) return;

    auto modDirInfo = modDirList.at(row - 1);
    auto dialog = new LocalModBrowserSettingsDialog(this, modDirInfo);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &newInfo){
        //exclude curseforge page
        modDirList[row - 1] = newInfo;
        dirWidgetItemList[row - 1]->setText(newInfo.getGameVersion());
        localModBrowserList[row - 1]->setModDirInfo(newInfo);
    });
    connect(updateVersionsWatcher, &QFutureWatcher<void>::finished, dialog, &LocalModBrowserSettingsDialog::updateVersions);
    dialog->exec();
}

