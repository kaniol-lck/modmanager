#include "downloadmanager.h"

#include "config.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    DOWNLOAD_COUNT(Config().getDownloadCount())
{

}

DownloadManager *DownloadManager::manager()
{
    static DownloadManager manager;
    return &manager;
}

void DownloadManager::tryDownload()
{
    int count = 0;
    for(auto downloader : qAsConst(downloadList)){
        if(downloader->getStatus() == ModDownloader::Downloading)
            count ++;
        //max download
        if(count >= DOWNLOAD_COUNT) return;
        if(downloader->getStatus() == ModDownloader::Queue){
            downloader->startDownload();
            count ++;
        }
    }
}

ModDownloader *DownloadManager::addModDownload(std::shared_ptr<DownloadFileInfo> info, QString path)
{
    auto downloader = new ModDownloader(this);
    downloader->downloadMod(info, path);
    addDownloader(downloader);
    tryDownload();
    return downloader;
}

ModDownloader *DownloadManager::addModupdate(std::shared_ptr<DownloadFileInfo> info, QString path, std::function<void ()> finishCallback)
{
    auto downloader = new ModDownloader(this);
    downloader->updateMod(info, path);
    connect(downloader, &Downloader::finished, this, finishCallback);
    addDownloader(downloader);
    tryDownload();
    return downloader;
}

void DownloadManager::addDownloader(ModDownloader *downloader)
{
    downloadList << downloader;
    connect(downloader, &ModDownloader::finished, this, &DownloadManager::tryDownload);
    emit downloaderAdded(downloader);
}

const QList<ModDownloader *> &DownloadManager::getDownloadList() const
{
    return downloadList;
}
