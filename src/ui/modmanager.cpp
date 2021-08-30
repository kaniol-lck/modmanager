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

    Config config;
    for(const auto &e : config.getDirList()){
        auto m = e.toMap();
        ModDirInfo dirInfo(QDir(m.value("dir").toString()), GameVersion(m.value("gameVersion").toString()), ModLoaderType::fromString(m.value("loaderType").toString()));
        modDirList << dirInfo;
    }

    //Curseforge
    auto curseforgeModBrowser = new CurseforgeModBrowser(this);
    ui->modDirSelectorWidget->addItem("Curseforge");
    ui->stackedWidget->addWidget(curseforgeModBrowser);

    //Modrinth
    auto modrinthModBrowser = new ModrinthModBrowser(this);
    ui->modDirSelectorWidget->addItem("Modrinth");
    ui->stackedWidget->addWidget(modrinthModBrowser);

    //Local
    for(const auto &modDirInfo : qAsConst(modDirList)){
        if(modDirInfo.exists()) {
            auto item = new QListWidgetItem(modDirInfo.getGameVersion() + " - " + ModLoaderType::toString(modDirInfo.getLoaderType()));
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

void ModManager::on_modDirSelectorWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void ModManager::on_newLocalBrowserButton_clicked()
{
    auto dialog = new LocalModBrowserSettingsDialog(this);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &modDirInfo){
        modDirList << modDirInfo;
        auto item = new QListWidgetItem(modDirInfo.getGameVersion() + " - " + ModLoaderType::toString(modDirInfo.getLoaderType()));
        auto localModBrowser = new LocalModBrowser(this, modDirInfo);
        dirWidgetItemList.append(item);
        localModBrowserList.append(localModBrowser);
        ui->modDirSelectorWidget->addItem(item);
        ui->stackedWidget->addWidget(localModBrowser);

        Config config;
        QList<QVariant> list;
        for(const auto &dirInfo : qAsConst(modDirList)){
            QMap<QString, QVariant> map;
            map["dir"] = dirInfo.getModDir().path();
            map["gameVersion"] = QString(dirInfo.getGameVersion());
            map["loaderType"] = ModLoaderType::toString(dirInfo.getLoaderType());
            list << QVariant::fromValue(map);
        }
        qDebug() << list;
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
        dirWidgetItemList[row - 2]->setText(newInfo.getGameVersion() + " - " + ModLoaderType::toString(modDirInfo.getLoaderType()));
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

