#include "qaria2.h"

#include <QtConcurrent>
#include <QTimer>
#include <QDebug>

#include "qaria2downloader.h"

int QAria2::downloadEventCallback(aria2::Session *session, aria2::DownloadEvent event, const aria2::A2Gid gid, void *userData)
{
    if(!qaria2()->downloaders_.contains(gid)) return 0;
    auto downloader = qaria2()->downloaders_.value(gid);
    downloader->setEvent(event);
    //callback
    return 0;
}

QAria2::QAria2(QObject *parent) : QObject(parent)
{
    aria2::libraryInit();
    config.downloadEventCallback = downloadEventCallback;
    session_ = aria2::sessionNew(aria2::KeyVals(), config);
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

void QAria2::run()  {
    if(isRunning_) return;
    isRunning_ = true;
    emit started();
    auto timer = new QTimer;
    //interval between refreshing info
    timer->start(500);
    auto conn = connect(timer, &QTimer::timeout, this, [=]{
        aria2::GlobalStat gstat = aria2::getGlobalStat(session_);
        emit downloadSpeed(gstat.downloadSpeed, gstat.uploadSpeed);
        emit globalDownloadStat(gstat.numWaiting, gstat.numActive, gstat.downloadSpeed, gstat.uploadSpeed);
        for (auto &&downloader : downloaders_)
            downloader->update();

    });
    auto watcher = new QFutureWatcher<int>(this);
    watcher->setFuture(QtConcurrent::run([=]{
        return aria2::run(session_, aria2::RUN_DEFAULT);
    }));
    connect(watcher, &QFutureWatcher<int>::finished, this, [=]{
        qDebug() << watcher->result();
        disconnect(conn);
        timer->deleteLater();
        isRunning_ = false;
        emit finished();
        for(const auto &downloader : qAsConst(downloaders_))
            downloader->deleteDownloadHandle();
        downloaders_.clear();
    });
}

const QMap<aria2::A2Gid, QAria2Downloader *> &QAria2::downloaders() const
{
    return downloaders_;
}

aria2::Session *QAria2::session() const
{
    return session_;
}

QAria2Downloader *QAria2::download(const QUrl &url, const QString &path)
{
    return download(new QAria2Downloader(url, path));
}

QAria2Downloader *QAria2::download(QAria2Downloader *downloader)
{
    qDebug() << "addDownload";
    std::vector<std::string> urls = { downloader->url().toString().toStdString() };
    aria2::KeyVals options;
    options.emplace_back("dir", downloader->path().toStdString());
    options.emplace_back("continue", "false");
    aria2::A2Gid gid = 0;
    int rv = aria2::addUri(session_, &gid, urls, options);
    run();
    if(rv < 0) {
        qDebug() << "Failed";
    }
    if(downloaders_.contains(gid)) return nullptr;
    downloader->setGid(gid);
    downloaders_[gid] = downloader;
    emit downloaderAdded(downloader);
    return downloader;
}
