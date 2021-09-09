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
    mod_(mod)
{
    //init ui
    ui->setupUi(this);
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->curseforgeButton->setEnabled(false);
    ui->modrinthButton->setEnabled(false);

    //init info
    updateInfo();

    //signals / slots
    connect(mod_, &LocalMod::curseforgeUpdateReady, this, &LocalModItemWidget::curseforgeUpdateReady);
    connect(mod_, &LocalMod::checkCurseforgeStarted, this, &LocalModItemWidget::startCheckCurseforge);
    connect(mod_, &LocalMod::curseforgeReady, this, &LocalModItemWidget::curseforgeReady);
    connect(mod_, &LocalMod::checkCurseforgeUpdateStarted, this, &LocalModItemWidget::startCheckCurseforgeUpdate);

    connect(mod_, &LocalMod::modrinthUpdateReady, this, &LocalModItemWidget::modrinthUpdateReady);
    connect(mod_, &LocalMod::checkModrinthStarted, this, &LocalModItemWidget::startCheckModrinth);
    connect(mod_, &LocalMod::modrinthReady, this, &LocalModItemWidget::modrinthReady);
    connect(mod_, &LocalMod::checkModrinthUpdateStarted, this, &LocalModItemWidget::startCheckModrinthUpdate);

    connect(mod_, &LocalMod::updateStarted, this, &LocalModItemWidget::startUpdate);
    connect(mod_, &LocalMod::updateProgress, this, &LocalModItemWidget::updateProgress);
    connect(mod_, &LocalMod::updateFinished, this, &LocalModItemWidget::finishUpdate);
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

void LocalModItemWidget::updateInfo()
{
    ui->modName->setText(mod_->modInfo().name());
    ui->modVersion->setText(mod_->modInfo().version());
    ui->modDescription->setText(mod_->modInfo().description());
    ui->modAuthors->setText(mod_->modInfo().authors().join("</b>, <b>").prepend("by <b>").append("</b>"));

    if(!mod_->modInfo().iconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(mod_->modInfo().iconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }
}

void LocalModItemWidget::on_updateButton_clicked()
{
    mod_->update(mod_->updateType());
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
    ui->updateProgress->setMaximum(mod_->updateCurseforgeFileInfo().value().size());
    ui->updateProgress->setVisible(true);
}

void LocalModItemWidget::updateProgress(qint64 bytesReceived)
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
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod);
    dialog->show();
}


void LocalModItemWidget::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod);
    dialog->show();
}

