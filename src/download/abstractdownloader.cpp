#include "abstractdownloader.h"

#include <QEventLoop>
#include <QFutureWatcher>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QtConcurrent>
#include <QDebug>

AbstractDownloader::AbstractDownloader(QObject *parent) :
    QObject(parent)
{}

AbstractDownloader::AbstractDownloader(QObject *parent, const DownloadFileInfo &info) :
    QObject(parent),
    info_(info)
{}

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
        if(!reply) continue;
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if(reply->error() != QNetworkReply::NoError) continue;

        //size_ = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        QUrl redirected;
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull())
            redirected = handleRedirect(redirection.toString());
        else
            redirected = url;
        reply->deleteLater();
        return redirected;
    }
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
