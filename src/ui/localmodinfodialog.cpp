#include "localmodinfodialog.h"
#include "ui_localmodinfodialog.h"

#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "curseforgemodinfodialog.h"
#include "modrinthmodinfodialog.h"

LocalModInfoDialog::LocalModInfoDialog(QWidget *parent, LocalMod *mod) :
    QDialog(parent),
    ui(new Ui::LocalModInfoDialog),
    mod_(mod)
{
    ui->setupUi(this);
    setWindowTitle(mod_->modInfo().fabric().name() + tr(" - Local"));

    ui->modName->setText(mod_->modInfo().fabric().name());
    ui->modVersion->setText(mod_->modInfo().fabric().version());
    ui->modDescription->setText(mod_->modInfo().fabric().description());

    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().fabric().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    ui->modIcon->setCursor(Qt::ArrowCursor);

    //update curseforge
    auto updateCurseforge = [=](bool bl){
        ui->curseforgeButton->setEnabled(bl);
    };

    if(mod->curseforgeMod() != nullptr)
        updateCurseforge(true);
    else{
        connect(mod_, &LocalMod::curseforgeReady, this, updateCurseforge);
    }

    //update modrinth
    auto updateModrinth = [=](bool bl){
        ui->modrinthButton->setEnabled(bl);
    };

    if(mod->modrinthMod() != nullptr)
        updateModrinth(true);
    else{
        connect(mod_, &LocalMod::modrinthReady, this, updateModrinth);
    }
}

LocalModInfoDialog::~LocalModInfoDialog()
{
    delete ui;
}

void LocalModInfoDialog::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod);
    dialog->show();
}


void LocalModInfoDialog::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod);
    dialog->show();
}

