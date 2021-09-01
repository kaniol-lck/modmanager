#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include "local/localmodinfo.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "curseforgemodinfodialog.h"
#include "modrinthmodinfodialog.h"
#include "util/tutil.hpp"

LocalModItemWidget::LocalModItemWidget(QWidget *parent, LocalMod *mod) :
    QWidget(parent),
    ui(new Ui::LocalModItemWidget),
    localMod(mod)
{
    //init ui
    ui->setupUi(this);
    ui->curseforgeButton->setIcon(QIcon(":/image/curseforge.svg"));
    ui->modrinthButton->setIcon(QIcon(":/image/modrinth.svg"));
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->curseforgeButton->setEnabled(false);
    ui->modrinthButton->setEnabled(false);

    //init info
    updateInfo();

    //signals / slots
    connect(localMod, &LocalMod::curseforgeUpdateReady, this, &LocalModItemWidget::curseforgeUpdateReady);
    connect(localMod, &LocalMod::checkCurseforgeStarted, this, &LocalModItemWidget::startCheckCurseforge);
    connect(localMod, &LocalMod::curseforgeReady, this, &LocalModItemWidget::curseforgeReady);
    connect(localMod, &LocalMod::checkCurseforgeUpdateStarted, this, &LocalModItemWidget::startCheckCurseforgeUpdate);

    connect(localMod, &LocalMod::modrinthUpdateReady, this, &LocalModItemWidget::modrinthUpdateReady);
    connect(localMod, &LocalMod::checkModrinthStarted, this, &LocalModItemWidget::startCheckModrinth);
    connect(localMod, &LocalMod::modrinthReady, this, &LocalModItemWidget::modrinthReady);
    connect(localMod, &LocalMod::checkModrinthUpdateStarted, this, &LocalModItemWidget::startCheckModrinthUpdate);

    connect(localMod, &LocalMod::updateStarted, this, &LocalModItemWidget::startUpdate);
    connect(localMod, &LocalMod::updateProgress, this, &LocalModItemWidget::updateProgress);
    connect(localMod, &LocalMod::updateFinished, this, &LocalModItemWidget::finishUpdate);
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
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

void LocalModItemWidget::curseforgeUpdateReady(bool need)
{
    if(need)
        ui->updateButton->setVisible(true);
}

void LocalModItemWidget::startCheckCurseforge()
{
    //TODO
}

void LocalModItemWidget::curseforgeReady(bool bl)
{
    ui->curseforgeButton->setEnabled(bl);
}

void LocalModItemWidget::startCheckCurseforgeUpdate()
{
    //TODO
}

void LocalModItemWidget::modrinthUpdateReady(bool need)
{
    if(need)
        ui->updateButton->setVisible(true);
}

void LocalModItemWidget::startCheckModrinth()
{
    //TODO
}

void LocalModItemWidget::modrinthReady(bool bl)
{
    ui->modrinthButton->setEnabled(bl);
}

void LocalModItemWidget::startCheckModrinthUpdate()
{
    //TODO
}

void LocalModItemWidget::startUpdate()
{
    ui->updateButton->setText(tr("Updating"));
    ui->updateButton->setEnabled(false);
    ui->updateProgress->setMaximum(localMod->getUpdateCurseforgeFileInfo().value().getFileLength());
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


void LocalModItemWidget::on_curseforgeButton_clicked()
{
    auto curseforgeMod = localMod->getCurseforgeMod();
    if(!curseforgeMod->getModInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod);
    dialog->show();
}


void LocalModItemWidget::on_modrinthButton_clicked()
{
    auto modrinthMod = localMod->getModrinthMod();
    if(!modrinthMod->getModInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod);
    dialog->show();
}

