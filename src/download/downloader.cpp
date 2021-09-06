#include "downloader.h"

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <algorithm>

#include "downloaderthread.h"
#include "config.h"
#include "util/funcutil.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    THREAD_COUNT(Config().getThreadCount()),
    bytesReceived_(THREAD_COUNT),
    bytesTotal_(THREAD_COUNT)
{
    connect(this, &Downloader::downloadInfoReady, this, &Downloader::startDownload, Qt::QueuedConnection);
}

bool Downloader::download(const QUrl &url, const QString &path, const QString &filename)
{
    url_ = url;

    QDir dir(path);

    //get name
    if(!filename.isEmpty())
        file_.setFileName(dir.absoluteFilePath(filename));
    else if(!url.fileName().isEmpty())
        file_.setFileName(dir.absoluteFilePath(url.fileName()));
    else
        file_.setFileName(dir.absoluteFilePath("index.html"));

    if(!file_.open(QIODevice::WriteOnly)) return false;

    emit downloadInfoReady();

    return true;
}

void Downloader::threadFinished(int /*index*/)
{
    finishedThreadCount_++;
//    qDebug() << finishedThreadCount << "/" << THREAD_COUNT;
    if(finishedThreadCount_ == THREAD_COUNT){
        emit finished();
        qDebug() << "finish:" << file_.fileName();
    }
}

void Downloader::updateProgress(int index, qint64 threadBytesReceived)
{
    bytesReceived_[index] = threadBytesReceived;

    auto bytesReceivedSum = std::accumulate(bytesReceived_.begin(), bytesReceived_.end(), 0);

    emit downloadProgress(bytesReceivedSum, size_);
}

void Downloader::startDownload()
{
    //handle redirect
    //this will update url and size
    handleRedirect();

    file_.resize(size_);
    //slice file into threads
    for(int i=0; i < THREAD_COUNT; i++){
        qint64 startPos = size_ * i / THREAD_COUNT;
        qint64 endPos = size_ * (i+1) / THREAD_COUNT;

        auto thread = new DownloaderThread(this);
        connect(thread, &DownloaderThread::threadFinished, this, &Downloader::threadFinished);
        connect(thread, &DownloaderThread::threadDownloadProgress, this, &Downloader::updateProgress);
        connect(thread, &DownloaderThread::threadErrorOccurred, [](int index, QNetworkReply::NetworkError code){
            qDebug() << index << code;
        });
        thread->download(i, url_, &file_, startPos, endPos);
    }
}

void Downloader::handleRedirect()
{
    int tryTimes = 3;
    while(tryTimes--)
    {
        QNetworkRequest request(url_);
        #ifndef QT_NO_SSL
        QSslConfiguration sslConfig = request.sslConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(sslConfig);
        #endif

        auto reply = accessManager()->head(request);
        if(!reply) continue;
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if(reply->error() != QNetworkReply::NoError) continue;

        //update size
        size_ = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        QVariant redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if(!redirection.isNull()){
            //update url
            url_ = redirection.toString();
            handleRedirect();
        }
        reply->deleteLater();
        return;
    }
}
