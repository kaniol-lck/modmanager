#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include <QDebug>

#include "curseforgemod.h"
#include "util/downloader.h"
#include "util/funcutil.h"

CurseforgeModItemWidget::CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    curseforgeMod(mod)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    ui->downloadSpeedText->setText(filesize2String(curseforgeMod->getLatestFileLength()));
    connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, &CurseforgeModItemWidget::updateThumbnail);

    ui->modName->setText(mod->getName());
    ui->modSummary->setText(mod->getSummary());
    ui->modAuthors->setText(mod->getAuthors().join(", ").prepend(tr("by ")));

    //set timer
    speedTimer.setInterval(1000 / 4);
    connect(&speedTimer, SIGNAL(timeout()), SLOT(updateDownlaodSpeed()));
}

CurseforgeModItemWidget::~CurseforgeModItemWidget()
{
    delete ui;
}

void CurseforgeModItemWidget::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(curseforgeMod->getThumbnailBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}

void CurseforgeModItemWidget::updateDownlaodSpeed()
{
    auto downloadBytes = ui->downloadProgress->value();
    auto bytes = (downloadBytes - lastDownloadBytes) * 4;
    lastDownloadBytes = downloadBytes;

    ui->downloadSpeedText->setText(filesize2String(bytes) + "/s");

}

void CurseforgeModItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    auto downloader = new Downloader(this);

    //download latest file by defsult
    //TODO: select file to download
    downloader->download(curseforgeMod->getLatestFileUrl(), curseforgeMod->getLatestFileName());
    ui->downloadProgress->setMaximum(curseforgeMod->getLatestFileLength());

    connect(downloader, &Downloader::downloadProgress, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
        ui->downloadProgress->setMaximum(curseforgeMod->getLatestFileLength());
    });
    connect(downloader, &Downloader::finished, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(filesize2String(curseforgeMod->getLatestFileLength()));
        ui->downloadButton->setText(tr("Downloaded"));
        speedTimer.stop();
        lastDownloadBytes = 0;
    });

    speedTimer.start();
}

