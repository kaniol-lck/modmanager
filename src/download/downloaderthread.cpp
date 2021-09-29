#include "downloaderthread.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include "util/funcutil.h"
#include "util/tutil.hpp"

DownloaderThread::DownloaderThread(QObject *parent, int index, const QUrl &url, QFile *file, qint64 startPos, qint64 endPos) :
    QObject(parent),
    index_(index),
    url_(url),
    file_(file),
    threadStartPos_(startPos),
    threadEndPos_(endPos)
{}

DownloaderThread::DownloaderThread(QObject *parent, const QUrl &url, QFile *file, const QVariant &variant) :
    QObject(parent),
    url_(url),
    file_(file)
{
    index_ = value(variant, "index").toInt();
    threadStartPos_ = value(variant, "startPos").toInt();
    threadEndPos_ = value(variant, "endPos").toInt();
    readySize_ = value(variant, "readySize").toInt();
}

QVariant DownloaderThread::toVariant() const
{
    QMap<QString, QVariant> map;
    map["index"] = index_;
    map["startPos"] = threadStartPos_;
    map["endPos"] = threadEndPos_;
    map["readySize"] = readySize_;

    return QVariant::fromValue(map);
}

void DownloaderThread::start()
{
    if(threadStartPos_ + readySize_ == threadEndPos_)
        return;
    //download range
    QNetworkRequest request;
    request.setUrl(url_);
    QString range = QString("bytes=%0-%1").arg(threadStartPos_ + readySize_).arg(threadEndPos_);
    request.setRawHeader("Range", range.toUtf8());

    stopped_ = false;
    reply_ = accessManager_.get(request);
    connect(reply_, &QNetworkReply::finished, this, [=]{
        if(stopped_) return;
        file_->flush();
        emit threadFinished(index_);
    });
    connect(reply_, &QNetworkReply::readyRead, this, &DownloaderThread::writeFile);
    connect(reply_, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code){
        if(stopped_) return ;
        emit threadErrorOccurred(index_, code);
    });
}

void DownloaderThread::stop()
{
    stopped_ = true;
    reply_->abort();
    file_->flush();
    reply_->deleteLater();
}

void DownloaderThread::writeFile()
{
    //TODO:
    //some download does not follow endPos
    //QNetworkReplyImplPrivate::error: Internal problem, this method must only be called once.
    const QByteArray &buffer = reply_->readAll();
    file_->seek(threadStartPos_ + readySize_);
    if(file_->write(buffer) == -1)
        qDebug() << index_;
    readySize_ += buffer.size();
    if(threadStartPos_ + readySize_ > threadEndPos_) return;
    emit threadDownloadProgress(index_, readySize_);
}
