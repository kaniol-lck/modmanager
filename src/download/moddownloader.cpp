#include "moddownloader.h"

#include <QDebug>

ModDownloader::ModDownloader(QObject *parent) : Downloader(parent)
{
    connect(this, &Downloader::sizeUpdated, this, [=](qint64 size){
        fileInfo_.size_ = size;
    });
}

void ModDownloader::downloadMod(const DownloadFileInfo &info)
{
    fileInfo_ = info;
    addDownload(fileInfo_.url(), fileInfo_.path(), fileInfo_.fileName());
    type_ = DownloadType::Download;
}

void ModDownloader::updateMod(const DownloadFileInfo &info)
{
    fileInfo_ = info;
    addDownload(fileInfo_.url(), fileInfo_.path(), fileInfo_.fileName());
    type_ = DownloadType::Update;
}

const DownloadFileInfo &ModDownloader::fileInfo() const
{
    return fileInfo_;
}
