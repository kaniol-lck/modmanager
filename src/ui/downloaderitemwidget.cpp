#include "downloaderitemwidget.h"
#include "ui_downloaderitemwidget.h"

#include "download/moddownloader.h"
#include "util/funcutil.h"

DownloaderItemWidget::DownloaderItemWidget(QWidget *parent, ModDownloader *downloader) :
    QWidget(parent),
    ui(new Ui::DownloaderItemWidget),
    modDownlaoder(downloader)
{
    ui->setupUi(this);

    auto fileInfo = modDownlaoder->getFileInfo();
    ui->displayNameText->setText(fileInfo->getDisplayName());
    ui->downloadSizeText->setText(numberConvert(fileInfo->getSize(), "B"));

    QString linkText = fileInfo->getFileName();
    linkText = "<a href=%1>" + linkText + "</a>";
    ui->fileNameText->setText(linkText.arg(fileInfo->getUrl().toString()));

    refreshStatus();

    connect(modDownlaoder, &ModDownloader::statusChanged, this, &DownloaderItemWidget::refreshStatus);
    connect(modDownlaoder, &ModDownloader::downloadProgress, this, &DownloaderItemWidget::downloadProgress);
    connect(modDownlaoder, &ModDownloader::downloadSpeed, this, &DownloaderItemWidget::downloadSpeed);
    connect(modDownlaoder, &ModDownloader::finished, this, [=]{
        ui->downloadSpeedText->setVisible(false);
        ui->downloadProgress->setValue(ui->downloadProgress->maximum());
    });
}

DownloaderItemWidget::~DownloaderItemWidget()
{
    delete ui;
}

void DownloaderItemWidget::refreshStatus()
{
    switch (modDownlaoder->getStatus()) {
    case ModDownloader::Idol:
        ui->downloaStatus->setText(tr("Idol"));
        break;
    case ModDownloader::Queue:
        ui->downloaStatus->setText(tr("Queue"));
        break;
    case ModDownloader::Downloading:
        ui->downloaStatus->setText(tr("Downloading"));
        break;
    case ModDownloader::Paused:
        ui->downloaStatus->setText(tr("Paused"));
        break;
    case ModDownloader::Finished:
        ui->downloaStatus->setText(tr("Finished"));
        break;
    }
}

void DownloaderItemWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setMaximum(bytesTotal);
    ui->downloadProgress->setValue(bytesReceived);
}

void DownloaderItemWidget::downloadSpeed(qint64 bytesPerSec)
{
    ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
}
