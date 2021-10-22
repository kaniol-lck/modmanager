#include "abstractdownloader.h"

#include <QEventLoop>
#include <QFutureWatcher>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QtConcurrent>

AbstractDownloader::AbstractDownloader(QObject *parent) :
    QObject(parent)
{}

AbstractDownloader::AbstractDownloader(QObject *parent, const QUrl &url, const QString &path) :
    QObject(parent),
    url_(url),
    path_(path)
{}

void AbstractDownloader::handleRedirect()
{
    auto watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(QtConcurrent::run([=]{
        url_ = handleRedirect(url_);
    }));
    connect(watcher, &QFutureWatcher<void>::finished, this, [=]{
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
        static QNetworkAccessManager accessManager;
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
