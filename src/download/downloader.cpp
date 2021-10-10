#include "downloader.h"

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <algorithm>

#include "downloaderthread.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/tutil.hpp"

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    threadCount_(Config().getThreadCount())
{
    connect(this, &Downloader::waitForRedirect, this, [=]{
        handleRedirect();
        emit sizeUpdated(size_);
        setStatus(Queue);
    }, Qt::QueuedConnection);

    //setup timer
    speedTimer_.setInterval(1000);
    connect(&speedTimer_, &QTimer::timeout, this, [=]{
        auto bytes = currentDownloadBytes_ - lastDownloadBytes_;
        lastDownloadBytes_ = currentDownloadBytes_;
        //TODO: average speed;
        emit downloadSpeed(bytes);
    });
}

Downloader::Downloader(QObject *parent, const QVariant &variant) :
    QObject(parent)
{
    threadCount_ = value(variant, "threadCount").toInt();
    bytesReceived_.resize(threadCount_);
    bytesTotal_.resize(threadCount_);
    url_ = value(variant, "url").toUrl();
    file_.setFileName(value(variant, "file").toString());
    size_ = value(variant, "size").toInt();
    for(const auto &v : value(variant, "threads").toList()){
        qDebug() << v;
        threads_.push_back(new DownloaderThread(this, url_, &file_, v));
    }
    status_ = static_cast<DownloadStatus>(value(variant, "status").toInt());
//    if(status_ == DownloadStatus::Perparing)
//        emit waitForRedirect();
}

QVariant Downloader::toVariant() const
{
    QMap<QString, QVariant> map;
    map["threadCount"] = threadCount_;
    map["url"] = url_;
    map["file"] = file_.fileName();
    map["size"] = size_;
    map["status"] = status_;
    QList<QVariant> threadList;
    for(const auto &thread : threads_)
        threadList << thread->toVariant();
    map["threads"] = threadList;

    return QVariant::fromValue(map);
}

void Downloader::addDownload(const QUrl &url, const QString &path, const QString &fileName)
{
    url_ = url;
    QDir dir(path);
    QString filePath;

    //get name
    if(!fileName.isEmpty())
        filePath = dir.absoluteFilePath(fileName);
    else if(!url.fileName().isEmpty())
        filePath = dir.absoluteFilePath(url.fileName());
    else
        filePath = dir.absoluteFilePath("index.html");

    file_.setFileName(filePath + ".downloading");

    setStatus(DownloadStatus::Perparing);
    emit waitForRedirect();
}

void Downloader::threadFinished(int /*index*/)
{
    finishedThreadCount_++;
//    qDebug() << finishedThreadCount << "/" << THREAD_COUNT;
    if(finishedThreadCount_ == threadCount_){
        for(auto thread : qAsConst(threads_))
            thread->stop();
        file_.close();
        file_.rename(file_.fileName().remove(".downloading"));
        setStatus(DownloadStatus::Finished);
        speedTimer_.stop();
        emit finished();
        qDebug() << "finish:" << file_.fileName();
    }
}

void Downloader::updateProgress(int index, qint64 threadBytesReceived)
{
    bytesReceived_[index] = threadBytesReceived;
    currentDownloadBytes_ = std::accumulate(bytesReceived_.cbegin(), bytesReceived_.cend(), 0);
    emit downloadProgress(currentDownloadBytes_, size_);
//    qDebug() << currentDownloadBytes_ << size_;
}

void Downloader::setStatus(DownloadStatus newStatus)
{
    if (status_ == newStatus)
        return;
    status_ = newStatus;
    emit statusChanged(status_);
}

Downloader::DownloadStatus Downloader::status() const
{
    return status_;
}

bool Downloader::startDownload()
{
    if(status_ != DownloadStatus::Queue) return false;
    setStatus(DownloadStatus::Downloading);
    speedTimer_.start();
    if(!file_.open(QIODevice::WriteOnly)) return false;
    file_.resize(size_);
    auto count = size_ / (512 * 1024) + 1; // 512KiB
    if(count < threadCount_) threadCount_ = count;
    qDebug() << "Thread Count:" << threadCount_;
    threads_.resize(threadCount_);
    bytesReceived_.resize(threadCount_);
    bytesTotal_.resize(threadCount_);
    //slice file into threads
    for(int i=0; i < threadCount_; i++){
        qint64 startPos = size_ * i / threadCount_;
        qint64 endPos = size_ * (i+1) / threadCount_;

        auto thread = new DownloaderThread(this, i, url_, &file_, startPos, endPos);
        threads_[i] = thread;
        connect(thread, &DownloaderThread::threadFinished, this, &Downloader::threadFinished);
        connect(thread, &DownloaderThread::threadDownloadProgress, this, &Downloader::updateProgress);
        connect(thread, &DownloaderThread::threadErrorOccurred, this, [=](int index, QNetworkReply::NetworkError code){
            qDebug() << index << code;
            speedTimer_.stop();
            emit downloadSpeed(0);
            for(auto thread : qAsConst(threads_))
                thread->stop();
            file_.close();
            setStatus(Error);
        });
        thread->start();
    }
    return true;
}

void Downloader::pauseDownload()
{
    if(status_ != DownloadStatus::Downloading) return;
    setStatus(DownloadStatus::Paused);
    speedTimer_.stop();
    emit downloadSpeed(0);
    for(auto thread : qAsConst(threads_))
        thread->stop();
    file_.close();
}

bool Downloader::resumeDownload()
{
    if(status_ != DownloadStatus::Paused) return false;
    setStatus(DownloadStatus::Downloading);
    speedTimer_.start();
    if(!file_.open(QIODevice::WriteOnly)) return false;
    for(auto thread : qAsConst(threads_))
        thread->start();
    return true;
}

const QUrl &Downloader::url() const
{
    return url_;
}

const QFile &Downloader::file() const
{
    return file_;
}

qint64 Downloader::size() const
{
    return size_;
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
        static QNetworkAccessManager accessManager;
        auto reply = accessManager.head(request);
        if(!reply) continue;
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if(reply->error() != QNetworkReply::NoError) continue;

        //update size
        size_ = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        //TODO: some download link does not get redirected
        //bmclapi
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull()){
            //update url
            url_ = redirection.toString();
            handleRedirect();
        }
        reply->deleteLater();
        return;
    }
}
