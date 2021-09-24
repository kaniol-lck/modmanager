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
    connect(this, &Downloader::waitForRedirect, this, [=]{
        handleRedirect();
        readyDownload_ = true;
        emit sizeUpdated(size_);
        emit downloadInfoReady();
    }, Qt::QueuedConnection);
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

    emit waitForRedirect();
}

QString Downloader::filePath() const
{
    return file_.fileName();
}

void Downloader::threadFinished(int /*index*/)
{
    finishedThreadCount_++;
//    qDebug() << finishedThreadCount << "/" << THREAD_COUNT;
    if(finishedThreadCount_ == THREAD_COUNT){
        file_.close();
        file_.rename(file_.fileName().remove(".downloading"));
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

bool Downloader::readyDownload() const
{
    return readyDownload_;
}

bool Downloader::startDownload()
{
    if(!file_.open(QIODevice::WriteOnly)) return false;
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
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull()){
            //update url
            url_ = redirection.toString();
            handleRedirect();
        }
        reply->deleteLater();
        return;
    }
}
