#include "moddownloader.h"

#include <QDebug>

constexpr int TIMER_PER_SEC = 4;

ModDownloader::ModDownloader(QObject *parent) : Downloader(parent)
{
    connect(this, &Downloader::finished, this, [=]{
        setStatus(DownloadStatus::Finished);
        speedTimer_.stop();
    });

    connect(this, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        currentDownloadBytes_ = bytesReceived;
    });

    //set timer
    speedTimer_.setInterval(1000 / TIMER_PER_SEC);
    connect(&speedTimer_, &QTimer::timeout, this, [=]{
        auto bytes = currentDownloadBytes_ - lastDownloadBytes_;
        lastDownloadBytes_ = currentDownloadBytes_;

        downloadBytes_ << bytes;
        if(downloadBytes_.size() > 4) downloadBytes_.pop_front();

        auto aver = std::accumulate(downloadBytes_.cbegin(), downloadBytes_.cend(), 0) / downloadBytes_.size();

        emit downloadSpeed(aver * TIMER_PER_SEC);
    });
}

void ModDownloader::downloadMod(std::shared_ptr<DownloadFileInfo> info, QString path)
{
    fileInfo_ = info;
    filePath_ = path;
    setStatus(DownloadStatus::Queue);
    type_ = DownloadType::Download;
}

void ModDownloader::updateMod(std::shared_ptr<DownloadFileInfo> info, QString path)
{
    fileInfo_ = info;
    filePath_ = path;
    setStatus(DownloadStatus::Queue);
    type_ = DownloadType::Update;
}

void ModDownloader::startDownload()
{
    setStatus(DownloadStatus::Downloading);
    speedTimer_.start();
    download(fileInfo_->url(), filePath_, fileInfo_->fileName());
}

const std::shared_ptr<DownloadFileInfo> &ModDownloader::fileInfo() const
{
    return fileInfo_;
}

ModDownloader::DownloadStatus ModDownloader::status() const
{
    return status_;
}

ModDownloader::DownloadType ModDownloader::type() const
{
    return type_;
}

const QString &ModDownloader::readySize() const
{
    return readySize_;
}

const QString &ModDownloader::filePath() const
{
    return filePath_;
}

void ModDownloader::setStatus(DownloadStatus newStatus)
{
    if (status_ == newStatus)
        return;
    status_ = newStatus;
    emit statusChanged();
}
