#include "qaria2downloaderitemwidget.h"
#include "ui_qaria2downloaderitemwidget.h"

#include <QDebug>
#include <QClipboard>
#include <QMenu>

#include "download/qaria2downloader.h"
#include "util/funcutil.h"

QAria2DownloaderItemWidget::QAria2DownloaderItemWidget(QWidget *parent, QAria2Downloader *downloader, const DownloadFileInfo &info) :
    QWidget(parent),
    ui(new Ui::QAria2DownloaderItemWidget),
    downloader_(downloader)
{
    ui->setupUi(this);
    ui->pauseButton->setVisible(false);
    ui->filename->setText(info.fileName());

    if(!info.icon().isNull())
        ui->downloadIcon->setPixmap(info.icon().scaled(80, 80, Qt::KeepAspectRatio));

    if(!info.title().isEmpty())
        ui->displayNameText->setText(info.title() + R"( <span style="color:gray">()" + info.displayName() + ")</span>");
    else if(!info.title().isEmpty())
        ui->displayNameText->setText(info.title());
    else if(!info.displayName().isEmpty())
        ui->displayNameText->setText(info.displayName());
    else
        ui->displayNameText->setText("Unnamed");

    refreshStatus(downloader->status());
    downloadSpeed(0, 0);

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
        ui->pauseButton->setVisible(true);
        ui->pauseButton->setIcon(QIcon::fromTheme("media-playback-pause"));
        break;
    case aria2::DOWNLOAD_WAITING:
        text = tr("Waiting");
        ui->pauseButton->setVisible(false);
        break;
    case aria2::DOWNLOAD_PAUSED:
        text = tr("Paused");
        ui->pauseButton->setVisible(true);
        ui->pauseButton->setIcon(QIcon::fromTheme("media-playback-start"));
        break;
    case aria2::DOWNLOAD_COMPLETE:
        text = tr("Complete");
        ui->pauseButton->setVisible(false);
        break;
    case aria2::DOWNLOAD_ERROR:
        text = tr("Error");
        ui->pauseButton->setVisible(false);
        break;
    case aria2::DOWNLOAD_REMOVED:
        text = tr("Removed");
        ui->pauseButton->setVisible(false);
        break;
    }
    ui->downloaStatus->setText(text);
}

void QAria2DownloaderItemWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setMaximum(bytesTotal);
    ui->downloadProgress->setValue(bytesReceived);
    ui->downloadSizeText->setText(sizeConvert(bytesTotal));
}

void QAria2DownloaderItemWidget::downloadSpeed(qint64 download, qint64 upload)
{
    ui->downloadSpeedText->setVisible(download);
    ui->uploadSpeedText->setVisible(upload);
    if(download)
        ui->downloadSpeedText->setText(speedConvert(download));
    if(upload)
        ui->uploadSpeedText->setText(speedConvert(upload));
}

void QAria2DownloaderItemWidget::on_pauseButton_clicked()
{
    if(downloader_->status() == aria2::DOWNLOAD_ACTIVE)
        downloader_->pause();
    else if(downloader_->status() == aria2::DOWNLOAD_PAUSED)
        downloader_->start();
}


void QAria2DownloaderItemWidget::on_QAria2DownloaderItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    menu->addAction(tr("Copy Download Link"), this, [=]{
        QApplication::clipboard()->setText(downloader_->url().toString());
    });
    menu->exec(mapToGlobal(pos));
}

