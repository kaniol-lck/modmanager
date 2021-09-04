#include "moddownloader.h"

ModDownloader::ModDownloader(QObject *parent) : Downloader(parent)
{
    connect(this, &Downloader::finished, this, [=]{
        setStatus(DownloadStatus::Finished);
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
