#include "downloadmanager.h"

#include "qaria2.h"
#include "qaria2downloader.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    qaria2_(QAria2::qaria2())
{}

DownloadManager *DownloadManager::manager()
{
    static DownloadManager manager;
    return &manager;
}

QAria2Downloader *DownloadManager::download(const DownloadFileInfo &info)
{
    auto downloader = new QAria2Downloader(info.url(), info.path());
    emit downloaderAdded(info, downloader);
    //handle redirect
    downloader->handleRedirect();
    connect(downloader, &AbstractDownloader::redirected, this, [=]{
        qaria2_->download(downloader);
    });
    return downloader;
}

QAria2 *DownloadManager::qaria2() const
{
    return qaria2_;
}
