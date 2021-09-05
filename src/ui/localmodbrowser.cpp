#include "localmodbrowser.h"
#include "ui_localmodbrowser.h"

#include <QDir>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>

#include "localmoditemwidget.h"
#include "localmodinfodialog.h"
#include "localmodupdatedialog.h"
#include "config.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, const ModDirInfo &info) :
    QWidget(parent),
    ui(new Ui::LocalModBrowser),
    modDirInfo_(info)
{
    ui->setupUi(this);

    //async load
    connect(this, &LocalModBrowser::modsDirUpdated, this, &LocalModBrowser::updateModList, Qt::QueuedConnection);

    emit modsDirUpdated();
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

void LocalModBrowser::updateModList()
{
    modList_.clear();
    ui->modListWidget->clear();
    const auto &list = modDirInfo_.modDir().entryInfoList(QDir::Files);
    for (const QFileInfo& entryInfo : list) {
        LocalModInfo modInfo(entryInfo.absoluteFilePath());
        if(!modInfo.isFabricMod()) continue;
        auto localMod = new LocalMod(this, modInfo);
        auto modItemWidget = new LocalModItemWidget(ui->modListWidget, localMod);

        //curseforge
        localMod->searchOnCurseforge();
        connect(localMod, &LocalMod::curseforgeReady, this, [=](bool bl){
            if(bl && Config().getAutoCheckUpdate()) localMod->checkCurseforgeUpdate(modDirInfo_.gameVersion().mainVersion(), modDirInfo_.loaderType());
        });

        //modrinth
        localMod->searchOnModrinth();
        connect(localMod, &LocalMod::modrinthReady, this, [=](bool bl){
//            if(bl && Config().getAutoCheckUpdate()) localMod->checkModrinthUpdate(modDirInfo.getGameVersion().mainVersion(), modDirInfo.getLoaderType());
        });
        modList_ << localMod;

        auto *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(500, 100));

        ui->modListWidget->addItem(listItem);
        ui->modListWidget->setItemWidget(listItem, modItemWidget);

    }
}

void LocalModBrowser::on_modListWidget_currentRowChanged(int currentRow)
{
//    LocalModInfo modInfo = modList.at(currentRow);
//    QStringList stringList;
//    stringList << "id: " + modInfo.getId();
//    stringList << "version: " + modInfo.getVersion();
//    stringList << "name: " + modInfo.getName();
//    stringList << "description: " + modInfo.getDescription();
//    stringList << "sha1: " + modInfo.getSha1();
//    stringList << "murmurhash: " + modInfo.getMurmurhash();
//    stringList << "file modfication time: " + modInfo.getFileModificationTime().toString(Qt::DateFormat::DefaultLocaleLongDate);
//    ui->modInfoText->setText(stringList.join("\n"));
}

void LocalModBrowser::setModDirInfo(const ModDirInfo &newModDirInfo)
{
    modDirInfo_ = newModDirInfo;
    emit modsDirUpdated();
}


void LocalModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = modList_.at(index.row());
    auto dialog = new LocalModInfoDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_updateAllButton_clicked()
{
    auto dialog = new LocalModUpdateDialog(this, modList_);
    dialog->show();
}

