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
    speedTimer_.setInterval(1000 / TIMER_PER_SEC);
    speedTimer_.start();
    connect(&speedTimer_, &QTimer::timeout, this, [=]{
        auto bytes = std::accumulate(speedList_.cbegin(), speedList_.cend(), 0);
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
    for(auto downloader : qAsConst(downloadList_)){
        if(downloader->status() == ModDownloader::Downloading)
            count ++;
        //max download
        if(count >= DOWNLOAD_COUNT) return;
        if(downloader->status() == ModDownloader::Queue){
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
    auto index = downloadList_.size();
    downloadList_ << downloader;
    speedList_.append(0);
    connect(downloader, &ModDownloader::finished, this, &DownloadManager::tryDownload);
    connect(downloader, &ModDownloader::finished, this, [=]{
        speedList_[index] = 0;
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        speedList_[index] = bytesPerSec;
    });
    emit downloaderAdded(downloader);
}

const QList<ModDownloader *> &DownloadManager::downloadList() const
{
    return downloadList_;
}
