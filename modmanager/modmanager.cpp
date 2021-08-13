#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QDir>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QCryptographicHash>

#include "modmanager/modentrywidget.h"

ModManager::ModManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModManager)
{
    ui->setupUi(this);

    auto modDir = QDir("/run/media/kaniol/SanDisk/Minecraft/1.17.1/.minecraft/mods/");
    for (const QFileInfo& entryInfo : modDir.entryInfoList(QDir::Files)) {
        ModInfo modInfo(entryInfo.absoluteFilePath());
        if(!modInfo.isFabricMod()) continue;
        modList.append(modInfo);

//        ui->modListWidget->addItem(modInfo.getName());
        auto *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(500, 100));
        auto modEntryWidget = new ModEntryWidget(ui->modListWidget, modInfo);

        ui->modListWidget->addItem(listItem);
        ui->modListWidget->setItemWidget(listItem, modEntryWidget);

    }
}

ModManager::~ModManager()
{
    delete ui;
}


void ModManager::on_modListWidget_currentRowChanged(int currentRow)
{
    ModInfo modInfo = modList.at(currentRow);
    QStringList stringList;
    stringList << "id: " + modInfo.getId();
    stringList << "version: " + modInfo.getVersion();
    stringList << "name: " + modInfo.getName();
    stringList << "description: " + modInfo.getDescription();
    stringList << "sha1: " + modInfo.getSha1();
    stringList << "murmurhash: " + modInfo.getMurmurhash();
    ui->modInfoText->setText(stringList.join("\n"));
}

