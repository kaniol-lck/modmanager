#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include "local/localmodinfo.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "curseforgemodinfodialog.h"
#include "util/tutil.hpp"

LocalModItemWidget::LocalModItemWidget(QWidget *parent, LocalMod *mod) :
    QWidget(parent),
    ui(new Ui::LocalModItemWidget),
    localMod(mod)
{
    ui->setupUi(this);
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);

    ui->modName->setText(mod->getModInfo().getName());
    ui->modVersion->setText(mod->getModInfo().getVersion());
    ui->modDescription->setText(mod->getModInfo().getDescription());

    if(!mod->getModInfo().getIconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(mod->getModInfo().getIconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }

    connect(localMod, &LocalMod::startCheckUpdate, this, [=]{
        ui->updateButton->setVisible(true);
    });

    //start update
    connect(localMod, &LocalMod::startUpdate, this, [=]{
        ui->updateButton->setText(tr("Updating"));
        ui->updateButton->setEnabled(false);
        ui->updateProgress->setMaximum(localMod->getUpdateFileInfo().value().getFileLength());
        ui->updateProgress->setVisible(true);
    });

    //update progress
    connect(localMod, &LocalMod::updateProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->updateProgress->setValue(bytesReceived);
    });

    //finish update
    connect(localMod, &LocalMod::updateFinished, this, [=]{
        ui->updateProgress->setVisible(false);
        ui->updateButton->setText(tr("Updated"));

        //reset speed timer
    });
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

void LocalModItemWidget::needUpdate(bool need)
{
    if(need){
        ui->updateButton->setText(tr("Update"));
        ui->updateButton->setEnabled(true);
    }
    else
        ui->updateButton->setVisible(false);
}

void LocalModItemWidget::on_updateButton_clicked()
{
    localMod->update(false);
}

