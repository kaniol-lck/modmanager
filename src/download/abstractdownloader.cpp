#include "abstractdownloader.h"

#include <QEventLoop>
#include <QFutureWatcher>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QtConcurrent>
#include <QDebug>

AbstractDownloader::AbstractDownloader(QObject *parent, const DownloadFileInfo &info) :
    QObject(parent),
    info_(info)
{
    connect(this, &AbstractDownloader::downloadSpeed, [=](qint64 download, qint64 upload){
        downSpeed_ = download;
        upSpeed_ = upload;
    });
    //update speed point every 1s
    timer_.start(1000);
    connect(&timer_, &QTimer::timeout, this, [=]{
        if(isStarted())
            addData(downSpeed_, upSpeed_);
        else
            addData(0, 0);
    });
}

AbstractDownloader::~AbstractDownloader()
{}

void AbstractDownloader::handleRedirect()
{
    auto watcher = new QFutureWatcher<QUrl>(this);
    watcher->setFuture(QtConcurrent::run([=]{
        return AbstractDownloader::handleRedirect(info_.url());
    }));
    connect(watcher, &QFutureWatcher<QUrl>::finished, this, [=]{
        info_.setUrl(watcher->result());
        emit infoChanged();
        emit redirected(info_.url());
    });
}

QUrl AbstractDownloader::handleRedirect(const QUrl &url)
{
    int tryTimes = 3;
    while(tryTimes--)
    {
        QNetworkRequest request(url);
#ifndef QT_NO_SSL
        QSslConfiguration sslConfig = request.sslConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(sslConfig);
#endif
        QNetworkAccessManager accessManager;
        auto reply = accessManager.head(request);
        qDebug() << "redirect:" << url;
        if(!reply) continue;
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if(reply->error() != QNetworkReply::NoError) continue;

        //size_ = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        QUrl redirected;
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull()/* && redirection.toUrl() == url*/)
            redirected = handleRedirect(redirection.toUrl());
        else
            redirected = url;
        reply->deleteLater();
        qDebug() << "rediect ended";
        return redirected;
    }
    qDebug() << "rediect ended.";
    return url;
}

const DownloadFileInfo &AbstractDownloader::info() const
{
    return info_;
}

void AbstractDownloader::setInfo(const DownloadFileInfo &newInfo)
{
    info_ = newInfo;
    emit infoChanged();
}

void AbstractDownloader::setIcon(const QPixmap &newIcon)
{
    info_.setIcon(newIcon);
    emit infoChanged();
}

void AbstractDownloader::addData(qint64 downSpeed, qint64 upSpeed)
{
    PointData point;
    point.x = QDateTime::currentMSecsSinceEpoch() / 1000;
    point.y[Download] = downSpeed;
    point.y[Upload] = upSpeed;

    dataCollection_.push_back(point);

    while (dataCollection_.length() > DATA_MAXSIZE)
        dataCollection_.removeFirst();

    emit dataUpdated();
}

const QList<AbstractDownloader::PointData> &AbstractDownloader::dataCollection() const
{
    return dataCollection_;
}

void AbstractDownloader::setDataCollection(const QList<PointData> &newDataCollection)
{
    dataCollection_ = newDataCollection;
}
