#include "qaria2downloaderitemwidget.h"
#include "ui_qaria2downloaderitemwidget.h"

#include <QDebug>

#include "download/qaria2downloader.h"
#include "util/funcutil.h"

QAria2DownloaderItemWidget::QAria2DownloaderItemWidget(QWidget *parent, QAria2Downloader *downloader, const DownloadFileInfo &info) :
    QWidget(parent),
    ui(new Ui::QAria2DownloaderItemWidget),
    downloader_(downloader)
{
    qDebug() << info.icon();
    ui->setupUi(this);

    ui->displayNameText->setText(info.displayName());
    if(!info.icon().isNull())
        ui->downloadIcon->setPixmap(info.icon().scaled(80, 80, Qt::KeepAspectRatio));

    refreshStatus(downloader->status());

    connect(downloader_, &QAria2Downloader::statusChanged, this, &QAria2DownloaderItemWidget::refreshStatus);
    connect(downloader_, &AbstractDownloader::downloadProgress, this, &QAria2DownloaderItemWidget::downloadProgress);
    connect(downloader_, &AbstractDownloader::downloadSpeed, this, &QAria2DownloaderItemWidget::downloadSpeed);
    connect(downloader_, &AbstractDownloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
    });
}

QAria2DownloaderItemWidget::~QAria2DownloaderItemWidget()
{
    delete ui;
}

void QAria2DownloaderItemWidget::refreshStatus(int status)
{
    QString text;
    switch (status) {
    case aria2::DOWNLOAD_ACTIVE:
        text = tr("Active");
        break;
    case aria2::DOWNLOAD_WAITING:
        text = tr("Waiting");
        break;
    case aria2::DOWNLOAD_PAUSED:
        text = tr("Paused");
        break;
    case aria2::DOWNLOAD_COMPLETE:
        text = tr("Complete");
        break;
    case aria2::DOWNLOAD_ERROR:
        text = tr("Error");
        break;
    case aria2::DOWNLOAD_REMOVED:
        text = tr("Removed");
        break;
    }
    ui->downloaStatus->setText(text);
}

void QAria2DownloaderItemWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setMaximum(bytesTotal);
    ui->downloadProgress->setValue(bytesReceived);
    ui->downloadSizeText->setText(numberConvert(bytesTotal, "B"));
}

void QAria2DownloaderItemWidget::downloadSpeed(qint64 download, qint64 upload)
{
    ui->downloadSpeedText->setText(numberConvert(download, "B/s") + " " + numberConvert(upload, "B/s"));
}
