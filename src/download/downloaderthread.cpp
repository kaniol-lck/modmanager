#include "downloaderthread.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include "util/funcutil.h"

DownloaderThread::DownloaderThread(QObject *parent, int index, const QUrl &url, QFile *file, qint64 startPos, qint64 endPos) :
    QObject(parent),
    index_(index),
    url_(url),
    file_(file),
    threadStartPos_(startPos),
    threadEndPos_(endPos)
{}

void DownloaderThread::start()
{
    //download range
    QNetworkRequest request;
    request.setUrl(url_);
    QString range = QString("bytes=%0-%1").arg(threadStartPos_ + readySize_).arg(threadEndPos_);
    request.setRawHeader("Range", range.toUtf8());

    paused_ = false;
    reply_ = accessManager_.get(request);
    connect(reply_, &QNetworkReply::finished, this, [=]{
        if(paused_) return;
        file_->flush();
        emit threadFinished(index_);
    });
    connect(reply_, &QNetworkReply::downloadProgress, this, [=](qint64 /*bytesReceived*/, qint64 /*bytesTotal*/){
        emit threadDownloadProgress(index_, readySize_);
    });
    connect(reply_, &QNetworkReply::readyRead, this, &DownloaderThread::writeFile);
    connect(reply_, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code){
        if(paused_) return ;
        emit threadErrorOccurred(index_, code);
    });
}

void DownloaderThread::stop()
{
    paused_ = true;
    reply_->abort();
    file_->flush();
    reply_->deleteLater();
}

void DownloaderThread::writeFile()
{
    const QByteArray &buffer = reply_->readAll();
    file_->seek(threadStartPos_ + readySize_);
    file_->write(buffer);
    readySize_ += buffer.size();
}
