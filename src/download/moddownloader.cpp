#include "moddownloader.h"

#include <QDebug>

constexpr int TIMER_PER_SEC = 4;

ModDownloader::ModDownloader(QObject *parent) : Downloader(parent)
{
    connect(this, &Downloader::finished, this, [=]{
        setStatus(DownloadStatus::Finished);
        speedTimer.stop();
    });

    connect(this, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        currentDownloadBytes = bytesReceived;
    });

    //set timer
    speedTimer.setInterval(1000 / TIMER_PER_SEC);
    connect(&speedTimer, &QTimer::timeout, this, [=]{
        auto bytes = currentDownloadBytes - lastDownloadBytes;
        lastDownloadBytes = currentDownloadBytes;

        downloadBytes << bytes;
        if(downloadBytes.size() > 4) downloadBytes.pop_front();

        auto aver = std::accumulate(downloadBytes.cbegin(), downloadBytes.cend(), 0) / downloadBytes.size();

        emit downloadSpeed(aver * TIMER_PER_SEC);
    });
}

void ModDownloader::downloadMod(std::shared_ptr<DownloadFileInfo> info, QString path)
{
    fileInfo = info;
    filePath = path;
    setStatus(DownloadStatus::Queue);
    type = DownloadType::Download;
}

void ModDownloader::updateMod(std::shared_ptr<DownloadFileInfo> info, QString path)
{
    fileInfo = info;
    filePath = path;
    setStatus(DownloadStatus::Queue);
    type = DownloadType::Update;
}

void ModDownloader::startDownload()
{
    setStatus(DownloadStatus::Downloading);
    speedTimer.start();
    download(fileInfo->getUrl(), filePath, fileInfo->getFileName());
}

const std::shared_ptr<DownloadFileInfo> &ModDownloader::getFileInfo() const
{
    return fileInfo;
}

ModDownloader::DownloadStatus ModDownloader::getStatus() const
{
    return status;
}

ModDownloader::DownloadType ModDownloader::getType() const
{
    return type;
}

const QString &ModDownloader::getReadySize() const
{
    return readySize;
}

const QString &ModDownloader::getFilePath() const
{
    return filePath;
}

void ModDownloader::setStatus(DownloadStatus newStatus)
{
    if (status == newStatus)
        return;
    status = newStatus;
    emit statusChanged();
}
