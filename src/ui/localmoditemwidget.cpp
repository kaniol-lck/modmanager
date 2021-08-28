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
    //init ui
    ui->setupUi(this);
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->updateStatus->setVisible(false);

    //init info
    updateInfo();

    //signals / slots
    connect(localMod, &LocalMod::updateReady, this, &LocalModItemWidget::updateReady);
    connect(localMod, &LocalMod::checkCurseforgeStarted, this, &LocalModItemWidget::startCheckCurseforge);
    connect(localMod, &LocalMod::curseforgeReady, this, &LocalModItemWidget::curseforgeReady);
    connect(localMod, &LocalMod::checkUpdateStarted, this, &LocalModItemWidget::startCheckUpdate);
    connect(localMod, &LocalMod::updateStarted, this, &LocalModItemWidget::startUpdate);
    connect(localMod, &LocalMod::updateProgress, this, &LocalModItemWidget::updateProgress);
    connect(localMod, &LocalMod::updateFinished, this, &LocalModItemWidget::finishUpdate);
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

void LocalModItemWidget::updateReady(bool need)
{
    ui->updateStatus->setVisible(false);
    ui->updateButton->setVisible(need);
}

void LocalModItemWidget::updateInfo()
{
    ui->modName->setText(localMod->getModInfo().getName());
    ui->modVersion->setText(localMod->getModInfo().getVersion());
    ui->modDescription->setText(localMod->getModInfo().getDescription());
    ui->modAuthors->setText(localMod->getModInfo().getAuthors().join("</b>, <b>").prepend("by <b>").append("</b>"));

    if(!localMod->getModInfo().getIconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(localMod->getModInfo().getIconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }
}

void LocalModItemWidget::on_updateButton_clicked()
{
    localMod->update();
}

void LocalModItemWidget::startCheckCurseforge()
{
    ui->updateStatus->setVisible(true);
    ui->updateStatus->setText(tr("Checking curseforge..."));
}

void LocalModItemWidget::curseforgeReady()
{
    ui->updateStatus->setVisible(false);
}

void LocalModItemWidget::startCheckUpdate()
{
    ui->updateStatus->setVisible(true);
    ui->updateStatus->setText(tr("Checking update..."));
}

void LocalModItemWidget::startUpdate()
{
    ui->updateButton->setText(tr("Updating"));
    ui->updateButton->setEnabled(false);
    ui->updateProgress->setMaximum(localMod->getUpdateFileInfo().value().getFileLength());
    ui->updateProgress->setVisible(true);
}

void LocalModItemWidget::updateProgress(qint64 bytesReceived, qint64 /*bytesTotal*/)
{
    ui->updateProgress->setValue(bytesReceived);
}

void LocalModItemWidget::finishUpdate()
{
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);

    //update info
    updateInfo();
}

