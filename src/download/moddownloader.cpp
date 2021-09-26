#include "moddownloader.h"

#include <QDebug>

constexpr int TIMER_PER_SEC = 1;

ModDownloader::ModDownloader(QObject *parent) : Downloader(parent)
{
    connect(this, &Downloader::sizeUpdated, this, [=](qint64 size){
        fileInfo_.size_ = size;
    });
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
        //TODO: average speed;
        emit downloadSpeed(bytes * TIMER_PER_SEC);
    });
}

void ModDownloader::downloadMod(const DownloadFileInfo &info)
{
    fileInfo_ = info;
    addDownload(fileInfo_.url(), fileInfo_.path(), fileInfo_.fileName());
    setStatus(DownloadStatus::Queue);
    type_ = DownloadType::Download;
}

void ModDownloader::updateMod(const DownloadFileInfo &info)
{
    fileInfo_ = info;
    addDownload(fileInfo_.url(), fileInfo_.path(), fileInfo_.fileName());
    setStatus(DownloadStatus::Queue);
    type_ = DownloadType::Update;
}

void ModDownloader::startDownload()
{
    if(status_ != DownloadStatus::Queue) return;
    setStatus(DownloadStatus::Downloading);
    speedTimer_.start();
    Downloader::startDownload();
}

void ModDownloader::pauseDownload()
{
    if(status_ != DownloadStatus::Downloading) return;
    setStatus(DownloadStatus::Paused);
    speedTimer_.stop();
    Downloader::pauseDownload();
}

bool ModDownloader::resumeDownload()
{
    if(status_ != DownloadStatus::Paused) return false;
    setStatus(DownloadStatus::Downloading);
    speedTimer_.start();
    return Downloader::resumeDownload();
}

const DownloadFileInfo &ModDownloader::fileInfo() const
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

void ModDownloader::setStatus(DownloadStatus newStatus)
{
    if (status_ == newStatus)
        return;
    status_ = newStatus;
    emit statusChanged();
}
