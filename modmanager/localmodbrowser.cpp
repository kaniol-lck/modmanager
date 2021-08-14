#include "localmodbrowser.h"
#include "ui_modbrowserwidget.h"

#include <QDir>
#include <QDateTime>

#include "localmoditemwidget.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, QDir path) :
    QWidget(parent),
    ui(new Ui::LocalModBrowser),
    modFolderPath(path)
{
    ui->setupUi(this);

    for (const QFileInfo& entryInfo : modFolderPath.entryInfoList(QDir::Files)) {
        LocalModInfo modInfo(entryInfo.absoluteFilePath());
        if(!modInfo.isFabricMod()) continue;
        modList.append(modInfo);

        auto *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(500, 100));
        auto modEntryWidget = new LocalModItemWidget(ui->modListWidget, modInfo);

        ui->modListWidget->addItem(listItem);
        ui->modListWidget->setItemWidget(listItem, modEntryWidget);

    }
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

void LocalModBrowser::on_modListWidget_currentRowChanged(int currentRow)
{
    LocalModInfo modInfo = modList.at(currentRow);
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

