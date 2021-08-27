#include "downloader.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <algorithm>

#include "downloaderthread.h"
#include "util/funcutil.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    bytesReceived(THREAD_COUNT),
    bytesTotal(THREAD_COUNT)
{

}

bool Downloader::download(const QUrl &url, const QDir &path, const QString &filename)
{
    downloadUrl = url;

    qDebug() << "url:" << downloadUrl;

    //get name
    if(!filename.isEmpty())
        downloadFile.setFileName(path.absoluteFilePath(filename));
    else if(!downloadUrl.fileName().isEmpty())
        downloadFile.setFileName(path.absoluteFilePath(downloadUrl.fileName()));
    else
        downloadFile.setFileName(path.absoluteFilePath("index.html"));

    qDebug() << "file name:" << downloadFile.fileName();

    if(!downloadFile.open(QIODevice::WriteOnly)) return false;

    //handle redirect
    //this will update url and size
    handleRedirect();

    downloadFile.resize(downloadSize);

    qDebug() << "file size:" << downloadSize;

    //slice file into threads
    for(int i=0; i < THREAD_COUNT; i++){
        qint64 startPos = downloadSize * i / THREAD_COUNT;
        qint64 endPos = downloadSize * (i+1) / THREAD_COUNT;

        auto thread = new DownloaderThread(this);
        connect(thread, &DownloaderThread::threadFinished, this, &Downloader::threadFinished);
        connect(thread, &DownloaderThread::threadDownloadProgress, this, &Downloader::updateProgress);
        connect(thread, &DownloaderThread::threadErrorOccurred, [](int index, QNetworkReply::NetworkError code){
            qDebug() << index << code;
        });
        thread->download(i, downloadUrl, &downloadFile, startPos, endPos);
    }

    return true;
}

void Downloader::threadFinished(int index)
{
    finishedThreadCount++;
//    qDebug() << finishedThreadCount << "/" << THREAD_COUNT;
    if(finishedThreadCount == THREAD_COUNT)
        emit finished();
}

void Downloader::updateProgress(int index, qint64 threadBytesReceived, qint64 threadBytesTotal)
{
    bytesReceived[index] = threadBytesReceived;
    bytesTotal[index] = threadBytesTotal;

    auto bytesReceivedSum = std::accumulate(bytesReceived.begin(), bytesReceived.end(), 0);
    auto bytesTotalSum = std::accumulate(bytesTotal.begin(), bytesTotal.end(), 0);

    emit downloadProgress(bytesReceivedSum, bytesTotalSum);
}

void Downloader::handleRedirect()
{
    int tryTimes = 3;
    while(tryTimes--)
    {
        QNetworkRequest request(downloadUrl);
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
        downloadSize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        QVariant redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if(!redirection.isNull()){
            //update url
            downloadUrl = redirection.toString();
            handleRedirect();
        }
        reply->deleteLater();
        return;
    }
}
