#include "curseforgemodinfodialog.h"
#include "ui_curseforgemodinfodialog.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "curseforge/curseforgemod.h"

CurseforgeModInfoDialog::CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModInfoDialog),
    curseforgeMod(mod),
    accessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle(mod->getModInfo().getName());

    ui->modName->setText(mod->getModInfo().getName());
    ui->modSummary->setText(mod->getModInfo().getSummary());
    ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->getModInfo().getWebsiteUrl().toString()));
    ui->modAuthors->setText(mod->getModInfo().getAuthors().join(", ").prepend(tr("by ")));

    if(!curseforgeMod->getModInfo().getThumbnailBytes().isEmpty()){
        updateThumbnail();
    } else {
        connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, &CurseforgeModInfoDialog::updateThumbnail);
    }

    if(!curseforgeMod->getModInfo().getDescription().isEmpty()){
        ui->modDescription->setText(curseforgeMod->getModInfo().getDescription());
    } else{
        mod->getDescription();
        connect(mod, &CurseforgeMod::descriptionReady,this, [=]{
            ui->modDescription->setText(curseforgeMod->getModInfo().getDescription());
        });
    }
}

CurseforgeModInfoDialog::~CurseforgeModInfoDialog()
{
    delete ui;
}

void CurseforgeModInfoDialog::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(curseforgeMod->getModInfo().getThumbnailBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}
