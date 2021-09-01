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
    localMod(mod)
{
    ui->setupUi(this);
    setWindowTitle(localMod->getModInfo().getName() + tr(" - Local"));
    ui->curseforgeButton->setIcon(QIcon(":/image/curseforge.svg"));
    ui->modrinthButton->setIcon(QIcon(":/image/modrinth.svg"));

    ui->modName->setText(localMod->getModInfo().getName());
    ui->modVersion->setText(localMod->getModInfo().getVersion());
    ui->modDescription->setText(localMod->getModInfo().getDescription());

    QPixmap pixelmap;
    pixelmap.loadFromData(localMod->getModInfo().getIconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    ui->modIcon->setCursor(Qt::ArrowCursor);

    //update curseforge
    auto updateCurseforge = [=](bool bl){
        ui->curseforgeButton->setEnabled(bl);
    };

    if(mod->getCurseforgeMod() != nullptr)
        updateCurseforge(true);
    else{
        connect(localMod, &LocalMod::curseforgeReady, this, updateCurseforge);
    }

    //update modrinth
    auto updateModrinth = [=](bool bl){
        ui->modrinthButton->setEnabled(bl);
    };

    if(mod->getModrinthMod() != nullptr)
        updateModrinth(true);
    else{
        connect(localMod, &LocalMod::modrinthReady, this, updateModrinth);
    }
}

LocalModInfoDialog::~LocalModInfoDialog()
{
    delete ui;
}

void LocalModInfoDialog::on_curseforgeButton_clicked()
{
    auto curseforgeMod = localMod->getCurseforgeMod();
    if(!curseforgeMod->getModInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod);
    dialog->show();
}


void LocalModInfoDialog::on_modrinthButton_clicked()
{
    auto modrinthMod = localMod->getModrinthMod();
    if(!modrinthMod->getModInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod);
    dialog->show();
}

