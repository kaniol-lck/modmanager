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

AbstractDownloader::AbstractDownloader(QObject *parent, const QUrl &url, const QString &path) :
    QObject(parent),
    url_(url),
    path_(path)
{}

AbstractDownloader::~AbstractDownloader()
{}

void AbstractDownloader::handleRedirect()
{
    auto watcher = new QFutureWatcher<QUrl>(this);
    watcher->setFuture(QtConcurrent::run([=]{
        return AbstractDownloader::handleRedirect(url_);
    }));
    connect(watcher, &QFutureWatcher<QUrl>::finished, this, [=]{
        url_ = watcher->result();
        emit redirected(url_);
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
        //TODO: some download link does not get redirected
        //bmclapi
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

const QUrl &AbstractDownloader::url() const
{
    return url_;
}

const QString &AbstractDownloader::path() const
{
    return path_;
}
