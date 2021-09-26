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
    //load from config
//    Config config;
//    for(const auto &variant : config.getDownloaderList()){
//        qDebug() << variant;
//        auto downloader = new Downloader(this, variant);
//        downloadList_ << downloader;
//        auto index = downloadList_.size();
//        connect(downloader, &Downloader::statusChanged, this, &DownloadManager::tryDownload);
//        speedList_.append(0);
//        connect(downloader, &Downloader::finished, this, &DownloadManager::tryDownload);
//        connect(downloader, &Downloader::finished, this, [=]{
//            speedList_[index] = 0;
//        });
//        connect(downloader, &Downloader::downloadSpeed, this, [=](qint64 bytesPerSec){
//            speedList_[index] = bytesPerSec;
//        });
//        connect(downloader, &Downloader::downloadProgress, this, &DownloadManager::saveToConfig);
//        connect(downloader, &Downloader::statusChanged, this, &DownloadManager::saveToConfig);
//    }
//    qDebug() << downloadList_.size();
//    tryDownload();
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
        if(downloader->status() == Downloader::Downloading)
            count ++;
        //max download
        if(count >= DOWNLOAD_COUNT) return;
        if(downloader->status() == Downloader::Queue){
            downloader->startDownload();
            count ++;
        }
    }
}

ModDownloader *DownloadManager::addModDownload(const DownloadFileInfo &info)
{
    auto manager = DownloadManager::manager();
    auto downloader = new ModDownloader(manager);
    downloader->downloadMod(info);
    manager->addDownloader(downloader);
    manager->tryDownload();
    return downloader;
}

ModDownloader *DownloadManager::addModUpdate(const DownloadFileInfo &info, std::function<void ()> finishCallback)
{
    auto manager = DownloadManager::manager();
    auto downloader = new ModDownloader(manager);
    downloader->updateMod(info);
    connect(downloader, &Downloader::finished, manager, finishCallback);
    manager->addDownloader(downloader);
    manager->tryDownload();
    return downloader;
}

void DownloadManager::addDownloader(Downloader *downloader)
{
    auto index = downloadList_.size();
    downloadList_ << downloader;
    connect(downloader, &Downloader::statusChanged, DownloadManager::manager(), &DownloadManager::tryDownload);
    speedList_.append(0);
    connect(downloader, &Downloader::finished, this, &DownloadManager::tryDownload);
    connect(downloader, &Downloader::finished, this, [=]{
        speedList_[index] = 0;
    });
    connect(downloader, &Downloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        speedList_[index] = bytesPerSec;
    });
    connect(downloader, &Downloader::downloadProgress, this, &DownloadManager::saveToConfig);
    emit downloaderAdded(downloader);
}

const QList<Downloader *> &DownloadManager::downloadList() const
{
    return downloadList_;
}

void DownloadManager::saveToConfig()
{
    Config config;
    QList<QVariant> list;
    for(const auto &downloader : qAsConst(manager()->downloadList_)){
        if(downloader->status() != Downloader::Finished)
            list << downloader->toVariant();
    }
    config.setDownloaderList(list);
}
