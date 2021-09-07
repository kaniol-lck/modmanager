#include "localmodbrowser.h"
#include "ui_localmodbrowser.h"

#include <QDir>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>

#include "local/localmodpath.h"
#include "localmoditemwidget.h"
#include "localmodinfodialog.h"
#include "localmodupdatedialog.h"
#include "config.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    QWidget(parent),
    ui(new Ui::LocalModBrowser),
    modPath_(modPath)
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
    ui->modListWidget->clear();
    for (const auto &mod: modPath_->modList()) {
        auto modItemWidget = new LocalModItemWidget(ui->modListWidget, mod);

        //curseforge
        mod->searchOnCurseforge();
        connect(mod, &LocalMod::curseforgeReady, this, [=](bool bl){
            if(bl && Config().getAutoCheckUpdate()) mod->checkCurseforgeUpdate(modPath_->info().gameVersion(), modPath_->info().loaderType());
        });

        //modrinth
        mod->searchOnModrinth();
        connect(mod, &LocalMod::modrinthReady, this, [=](bool bl){
//            if(bl && Config().getAutoCheckUpdate()) localMod->checkModrinthUpdate(modDirInfo.getGameVersion(), modDirInfo.getLoaderType());
        });

        auto *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(500, 100));

        ui->modListWidget->addItem(listItem);
        ui->modListWidget->setItemWidget(listItem, modItemWidget);

    }
}

void LocalModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = modPath_->modList().at(index.row());
    auto dialog = new LocalModInfoDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_updateAllButton_clicked()
{
    auto dialog = new LocalModUpdateDialog(this, modPath_);
    dialog->show();
}

