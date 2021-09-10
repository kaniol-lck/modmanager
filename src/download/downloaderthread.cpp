#include "downloaderthread.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include "util/funcutil.h"

DownloaderThread::DownloaderThread(QObject *parent) :
    QObject(parent),
    accessManager_(new QNetworkAccessManager(this))
{}

void DownloaderThread::download(int index, const QUrl &url, QFile *file, qint64 startPos, qint64 endPos)
{
    file_ = file;
    threadStartPos_ = startPos;
    threadEndPos_ = startPos;

    //download range
    QNetworkRequest request;
    request.setUrl(url);
    QString range = QString("bytes=%0-%1").arg(startPos).arg(endPos);
    request.setRawHeader("Range", range.toUtf8());

    reply_ = accessManager_->get(request);
    connect(reply_, &QNetworkReply::finished, this, [=]{
        emit threadFinished(index);
    });
    connect(reply_, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        emit threadDownloadProgress(index, bytesReceived);
    });
    connect(reply_, &QNetworkReply::readyRead, this, &DownloaderThread::writeFile);
    connect(reply_, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code){
        emit threadErrorOccurred(index, code);
    });

}

void DownloaderThread::writeFile()
{
    const QByteArray &buffer = reply_->readAll();
    file_->seek(threadStartPos_ + readySize_);
    file_->write(buffer);
    readySize_ += buffer.size();
}
