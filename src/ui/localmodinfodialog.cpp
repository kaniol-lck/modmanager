#include "localmodinfodialog.h"
#include "ui_localmodinfodialog.h"

#include "local/localmod.h"
#include "curseforgemodinfodialog.h"

LocalModInfoDialog::LocalModInfoDialog(QWidget *parent, LocalMod *mod) :
    QDialog(parent),
    ui(new Ui::LocalModInfoDialog),
    localMod(mod)
{
    ui->setupUi(this);
    setWindowTitle(localMod->getModInfo().getName() + tr(" - Local"));

    ui->modName->setText(localMod->getModInfo().getName());
    ui->modDescription->setText(localMod->getModInfo().getDescription());

    QPixmap pixelmap;
    pixelmap.loadFromData(localMod->getModInfo().getIconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    ui->modIcon->setCursor(Qt::ArrowCursor);

    //update curseforge
    auto updateCurseforge = [=]{
            ui->curseforgeButton->setEnabled(true);
    };

    if(mod->getCurseforgeMod() != nullptr)
        updateCurseforge();
    else{
        connect(localMod, &LocalMod::curseforgeReady, this, updateCurseforge);
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

