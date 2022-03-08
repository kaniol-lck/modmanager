#include "qaria2.h"

#include <QTimer>
#include <QDebug>
#include <QtConcurrent>
#include <QMessageBox>

#include "qaria2downloader.h"
#include "util/mmlogger.h"
#include "config.hpp"

int QAria2::downloadEventCallback(aria2::Session *session[[maybe_unused]], aria2::DownloadEvent event, const aria2::A2Gid gid, void *userData[[maybe_unused]])
{
    for(auto &&downloader : qaria2()->downloaders()){
        if(downloader->gid_ == gid){
            downloader->setEvent(event);
            return 0;
        }
    }
    return 0;
}

QAria2::QAria2(QObject *parent) : SpeedRecorder(parent)
{
    if(auto code = aria2::libraryInit())
        QMessageBox::warning(0, tr("Aria2 Error"), tr("Aria2 downloader init failed, download function may not work.") +
                             "\n" + tr("Error code: %1").arg(code));
    config_.downloadEventCallback = downloadEventCallback;
    config_.keepRunning = true;
    aria2::KeyVals options;
    options.emplace_back(std::make_pair("check-certificate", "false"));
    session_ = aria2::sessionNew(options, config_);
    QtConcurrent::run([=]{
        aria2::run(session_, aria2::RUN_DEFAULT);
    });
    MMLogger() << "aria2 start running";
    //interval between refreshing info
    timer_.start(1000);
    connect(&timer_, &QTimer::timeout, this, [=]{
        aria2::GlobalStat gstat = aria2::getGlobalStat(session_);
        emit downloadSpeed(gstat.downloadSpeed, gstat.uploadSpeed);
        emit globalDownloadStat(gstat.numWaiting, gstat.numActive, gstat.downloadSpeed, gstat.uploadSpeed);
        for (auto &&downloader : downloaders_)
            downloader->update();
    });
    connect(qApp, &QCoreApplication::aboutToQuit, this, [=]{
        aria2::shutdown(session_);
        MMLogger() << "aria2 shutdown.";
    });
    updateOptions();
}

QAria2::~QAria2()
{
    for(const auto &downloader : qAsConst(downloaders_))
        downloader->deleteDownloadHandle();
    downloaders_.clear();
    aria2::sessionFinal(session_);
    aria2::libraryDeinit();
}

QAria2 *QAria2::qaria2()
{
    static QAria2 qaria2;
    return &qaria2;
}

void QAria2::updateOptions()
{
    Config config;
    aria2::KeyVals options;
    options.emplace_back("timeout", std::to_string(config.getAria2timeout()));
    options.emplace_back("max-tries", std::to_string(config.getAria2maxTries()));
    options.emplace_back("max-concurrent-downloads", std::to_string(config.getAria2maxConcurrentDownloads()));
    aria2::changeGlobalOption(session_, options);
}

const QList<QAria2Downloader *> &QAria2::downloaders() const
{
    return downloaders_;
}

aria2::Session *QAria2::session() const
{
    return session_;
}

QAria2Downloader *QAria2::downloadNoRedirect(const QUrl &url, const QString &path)
{
    auto downloader = new QAria2Downloader(DownloadFileInfo(url, path));
    return download(downloader);
}

QAria2Downloader *QAria2::download(const QUrl &url, const QString &path)
{
    auto downloader = new QAria2Downloader(DownloadFileInfo(url, path));
    //handle redirect
    downloader->handleRedirect();
    connect(downloader, &AbstractDownloader::redirected, this, [=]{
        download(downloader);
    });
    return downloader;
}

QAria2Downloader *QAria2::download(QAria2Downloader *downloader)
{
    std::vector<std::string> urls = { downloader->info().url().toString().toStdString() };
    aria2::KeyVals options;
    options.emplace_back("dir", downloader->info().path().toStdString());
    options.emplace_back("continue", "false");
    aria2::A2Gid gid = 0;
    int rv = aria2::addUri(session_, &gid, urls, options);
    if(rv < 0) {
        qDebug() << "Failed";
    }
    for(auto &&downloader : downloaders_)
        if(downloader->gid_ == gid)
            return nullptr;
    downloader->setGid(gid);
    downloaders_ << downloader;
    downloader->update();
    emit downloaderAdded(downloader);
    return downloader;
}
