#include "downloaderinfowidget.h"
#include "ui_downloaderinfowidget.h"

#include "download/abstractdownloader.h"
#include "download/qaria2downloader.h"
#include "util/funcutil.h"

DownloaderInfoWidget::DownloaderInfoWidget(QWidget *parent, SpeedRecorder *speedRecorder) :
    QWidget(parent),
    ui(new Ui::DownloaderInfoWidget),
    globalSpeedRecorder_(speedRecorder)
{
    ui->setupUi(this);
    ui->displayNameText->setProperty("class", "Title");
    if(globalSpeedRecorder_){
        ui->globalSpeedWidget->setSpeedRecorder(globalSpeedRecorder_);
        ui->stackedWidget->setCurrentIndex(0);
        onGlobalDataUpdated();
        connect(globalSpeedRecorder_, &SpeedRecorder::dataUpdated, this, &DownloaderInfoWidget::onGlobalDataUpdated);
    }
}

DownloaderInfoWidget::~DownloaderInfoWidget()
{
    delete ui;
}

void DownloaderInfoWidget::setDownloader(AbstractDownloader *downloader)
{
    downloader_ = downloader;
    emit downloaderChanged();
    if(!downloader_){
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);
    ui->speedWidget->setSpeedRecorder(downloader_);
    onInfoChanged();
    connect(this, &DownloaderInfoWidget::downloaderChanged, disconnecter(
                connect(downloader, &AbstractDownloader::infoChanged, this, &DownloaderInfoWidget::onInfoChanged)));
    if(auto qaria2downloader = qobject_cast<QAria2Downloader*>(downloader)){
        onStatusChanged(qaria2downloader->status());
        onHandleUpdated();
        connect(this, &DownloaderInfoWidget::downloaderChanged, disconnecter(
                    connect(qaria2downloader, &QAria2Downloader::statusChanged, this, &DownloaderInfoWidget::onStatusChanged),
                    connect(qaria2downloader, &QAria2Downloader::handleUpdated, this, &DownloaderInfoWidget::onHandleUpdated)));
    }
}

void DownloaderInfoWidget::onInfoChanged()
{
    ui->url->setText(downloader_->info().url().toString());
    ui->displayNameText->setText(downloader_->info().title() + R"( <span style="color:gray">()" + downloader_->info().displayName() + ")</span>");
    ui->filename->setText(downloader_->info().fileName());
    ui->path->setText(downloader_->info().path());
}

void DownloaderInfoWidget::onStatusChanged(int status)
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

void DownloaderInfoWidget::onHandleUpdated()
{
    auto qaria2downloader = qobject_cast<QAria2Downloader*>(downloader_);
    if(!qaria2downloader) return;
    auto handle = qaria2downloader->handle();
    if(!handle) return;
    ui->completedLength->setText(sizeConvert(handle->getCompletedLength()));
    ui->totalLength->setText(sizeConvert(handle->getTotalLength()));
    ui->downloadSpeed->setText(speedConvert(handle->getDownloadSpeed()));
    ui->uploadSpeed->setText(speedConvert(handle->getUploadSpeed()));
    ui->connections->setText(QString::number(handle->getConnections()));
    ui->numPieces->setText(QString::number(handle->getNumPieces()));
    ui->pieceLength->setText(QString::number(handle->getPieceLength()));
    ui->errorCode->setText(QString::number(handle->getErrorCode()));
}

void DownloaderInfoWidget::onGlobalDataUpdated()
{
    ui->globalDownloadSpeed->setText(speedConvert(globalSpeedRecorder_->downSpeed()));
    ui->globalUploadSpeed->setText(speedConvert(globalSpeedRecorder_->upSpeed()));
}
