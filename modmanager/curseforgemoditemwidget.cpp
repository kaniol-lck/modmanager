#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include "curseforgemod.h"
#include "util/downloader.h"

CurseforgeModItemWidget::CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    curseforgeMod(mod)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    ui->downloadSpeedText->setVisible(false);
    connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, &CurseforgeModItemWidget::updateThumbnail);

    ui->modName->setText(mod->getName());
    ui->modSummary->setText(mod->getSummary());
    ui->modAuthors->setText(mod->getAuthors().join(", ").prepend(tr("by ")));

    //set timer
    speedTimer.setInterval(1000);
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
    auto bytes = downloadBytes - lastDownloadBytes;
    lastDownloadBytes = downloadBytes;

    QString text;

    if(bytes < 850)
        text = QString::number(bytes, 'g', 3) + " B/s";
    else if(bytes < 850000)
        text = QString::number(bytes / 1000., 'g', 3) + " KiB/s";
    else
        text = QString::number(bytes / 1000000., 'g', 3) + " MiB/s";

    ui->downloadSpeedText->setText(text);

}

void CurseforgeModItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    ui->downloadSpeedText->setVisible(true);
    auto downloader = new Downloader(this);

    //need a url pre-process
    QString url(curseforgeMod->getLatestFileUrl().toString().replace("edge", "media"));
    downloader->download(url, curseforgeMod->getLatestFileName(), curseforgeMod->getLatestFileLength());
    connect(downloader, &Downloader::downloadProgress, [=](qint64 bytesReceived, qint64 bytesTotal){
        ui->downloadProgress->setValue(bytesReceived);
        ui->downloadProgress->setMaximum(bytesTotal);
    });
    connect(downloader, &Downloader::finished, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setVisible(false);
        ui->downloadButton->setText(tr("Downloaded"));
        speedTimer.stop();
        lastDownloadBytes = 0;
    });

    speedTimer.start();
}

