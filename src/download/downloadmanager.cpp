#include "downloadmanager.h"

#include <algorithm>
#include <QDebug>

#include "config.h"

constexpr int TIMER_PER_SEC = 4;

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    DOWNLOAD_COUNT(Config().getDownloadCount())
{
    //set timer
    speedTimer.setInterval(1000 / TIMER_PER_SEC);
    speedTimer.start();
    connect(&speedTimer, &QTimer::timeout, this, [=]{
        auto bytes = std::accumulate(speedList.cbegin(), speedList.cend(), 0);
        emit downloadSpeed(bytes);
    });
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
    auto manager = DownloadManager::manager();
    auto downloader = new ModDownloader(manager);
    downloader->downloadMod(info, path);
    manager->addDownloader(downloader);
    manager->tryDownload();
    return downloader;
}

ModDownloader *DownloadManager::addModupdate(std::shared_ptr<DownloadFileInfo> info, QString path, std::function<void ()> finishCallback)
{
    auto manager = DownloadManager::manager();
    auto downloader = new ModDownloader(manager);
    downloader->updateMod(info, path);
    connect(downloader, &Downloader::finished, manager, finishCallback);
    manager->addDownloader(downloader);
    manager->tryDownload();
    return downloader;
}

void DownloadManager::addDownloader(ModDownloader *downloader)
{
    auto index = downloadList.size();
    downloadList << downloader;
    speedList.append(0);
    connect(downloader, &ModDownloader::finished, this, &DownloadManager::tryDownload);
    connect(downloader, &ModDownloader::finished, this, [=]{
        speedList[index] = 0;
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        speedList[index] = bytesPerSec;
    });
    emit downloaderAdded(downloader);
}

const QList<ModDownloader *> &DownloadManager::getDownloadList() const
{
    return downloadList;
}
