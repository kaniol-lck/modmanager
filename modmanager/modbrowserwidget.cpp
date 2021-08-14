#include "modbrowserwidget.h"
#include "ui_modbrowserwidget.h"

#include <QDir>
#include <QDateTime>

#include "modentrywidget.h"

ModBrowserWidget::ModBrowserWidget(QWidget *parent, QDir path) :
    QWidget(parent),
    ui(new Ui::ModBrowserWidget),
    modFolderPath(path)
{
    ui->setupUi(this);

    for (const QFileInfo& entryInfo : modFolderPath.entryInfoList(QDir::Files)) {
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

ModBrowserWidget::~ModBrowserWidget()
{
    delete ui;
}

void ModBrowserWidget::on_modListWidget_currentRowChanged(int currentRow)
{
    ModInfo modInfo = modList.at(currentRow);
    QStringList stringList;
    stringList << "id: " + modInfo.getId();
    stringList << "version: " + modInfo.getVersion();
    stringList << "name: " + modInfo.getName();
    stringList << "description: " + modInfo.getDescription();
    stringList << "sha1: " + modInfo.getSha1();
    stringList << "murmurhash: " + modInfo.getMurmurhash();
    stringList << "file modfication time: " + modInfo.getFileModificationTime().toString(Qt::DateFormat::DefaultLocaleLongDate);
    ui->modInfoText->setText(stringList.join("\n"));
}

