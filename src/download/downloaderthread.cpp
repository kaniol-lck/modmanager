#include "downloaderthread.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include "util/funcutil.h"

DownloaderThread::DownloaderThread(QObject *parent) :
    QObject(parent)
{

}

void DownloaderThread::download(int index, const QUrl &url, QFile *file, qint64 startPos, qint64 endPos)
{
    downloadFile = file;
    threadStartPos = startPos;
    threadEndPos = startPos;

    //download range
    QNetworkRequest request;
    request.setUrl(url);
    QString range = QString("bytes=%0-%1").arg(startPos).arg(endPos);
    request.setRawHeader("Range", range.toUtf8());

    reply = accessManager()->get(request);
    connect(reply, &QNetworkReply::finished, [=]{
        emit threadFinished(index);
    });
    connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesReceived, qint64 bytesTotal){
        emit threadDownloadProgress(index, bytesReceived, bytesTotal);
    });
    connect(reply, &QNetworkReply::readyRead, this, &DownloaderThread::writeFile);
    connect(reply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError code){
        emit threadErrorOccurred(index, code);
    });

}

void DownloaderThread::writeFile()
{
    const QByteArray &buffer = reply->readAll();
    downloadFile->seek(threadStartPos + readySize);
    downloadFile->write(buffer);
    readySize += buffer.size();
}
