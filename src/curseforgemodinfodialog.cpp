#include "curseforgemodinfodialog.h"
#include "ui_curseforgemodinfodialog.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "curseforgemod.h"

CurseforgeModInfoDialog::CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModInfoDialog),
    curseforgeMod(mod),
    accessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle(mod->getName());

    ui->modName->setText(mod->getName());
    ui->modSummary->setText(mod->getSummary());
    ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->getWebsiteUrl().toString()));
    ui->modAuthors->setText(mod->getAuthors().join(", ").prepend(tr("by ")));

    if(!curseforgeMod->getThumbnailBytes().isEmpty()){
        updateThumbnail();
    } else {
        connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, &CurseforgeModInfoDialog::updateThumbnail);
    }

    if(!curseforgeMod->getDescription().isEmpty()){
        ui->modDescription->setText(curseforgeMod->getDescription());
    } else{
        QUrl url = QString("https://addons-ecs.forgesvc.net/api/v2/addon/%1/description")
                .arg(QString::number(mod->getId()));
        QNetworkRequest request(url);
        connect(accessManager, &QNetworkAccessManager::finished, this, &CurseforgeModInfoDialog::updateDescription);
        accessManager->get(request);
    }
}

CurseforgeModInfoDialog::~CurseforgeModInfoDialog()
{
    delete ui;
}

void CurseforgeModInfoDialog::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(curseforgeMod->getThumbnailBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}

void CurseforgeModInfoDialog::updateDescription(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    curseforgeMod->setDescription(reply->readAll());
    ui->modDescription->setText(curseforgeMod->getDescription());
}
