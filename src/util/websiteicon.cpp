#include "websiteicon.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtConcurrent/QtConcurrent>
#include <QEventLoop>
#include <optional>
#include <QDebug>
#include <QWebEnginePage>

WebsiteIcon::WebsiteIcon(QObject *parent) : QObject(parent)
{}

void WebsiteIcon::get(QUrl websiteUrl)
{
    auto future = QtConcurrent::run([=](){
        QEventLoop loop;

        //existed icon
        auto hostname = websiteUrl.host();
        if(hostname.contains("github.com"))
            return QIcon(":/image/github.svg");
        if(hostname.contains("curseforge.com"))
            return QIcon(":/image/curseforge.svg");
        if(hostname.contains("modrinth.com"))
            return QIcon(":/image/modrinth.svg");
        if(hostname.contains("fabricmc.net"))
            return QIcon(":/image/fabric.png");

        //favicon
        //TODO: doesn't work
        QUrl url;
        url.setScheme(websiteUrl.scheme());
        url.setHost(hostname);
        url.setPath("/favicon.ico");
        QNetworkRequest request(url);
        QNetworkAccessManager manager;
        auto reply = manager.get(request);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if(reply->error() == QNetworkReply::NoError){
            auto bytes = reply->readAll();
            reply->deleteLater();
            return QIcon(bytes);
        }

        //QWebEnginePage icon
        auto page = new QWebEnginePage;
        page->setUrl(websiteUrl);
        connect(page, &QWebEnginePage::iconChanged, &loop, &QEventLoop::quit);
        loop.exec();
        return page->icon();
    });

    auto watcher = new QFutureWatcher<QIcon>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<QIcon>::finished, this, [=]{
        auto icon = watcher->result();
        emit iconGot(icon);
    });
}
