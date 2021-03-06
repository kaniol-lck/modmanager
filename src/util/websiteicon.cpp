#include "websiteicon.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtConcurrent/QtConcurrent>
#include <QEventLoop>
#include <optional>
#include <QDebug>
//#include <QWebEnginePage>

WebsiteIcon::WebsiteIcon(QObject *parent) : QObject(parent)
{}

void WebsiteIcon::get(QUrl websiteUrl)
{
    auto future = QtConcurrent::run([=]{
        //existed icon
        auto hostname = websiteUrl.host();
        if(hostname.contains("github.com"))
            return QIcon(":/image/github.svg");
        if(hostname.contains("gitlab.com"))
            return QIcon(":/image/gitlab.svg");
        if(hostname.contains("curseforge.com"))
            return QIcon(":/image/curseforge.svg");
        if(hostname.contains("modrinth.com"))
            return QIcon(":/image/modrinth.svg");
        if(hostname.contains("fabricmc.net"))
            return QIcon(":/image/fabric.png");
        if(hostname.contains("replaymod.com"))
            return QIcon(":/image/replay.png");
        if(hostname.contains("optifine.net"))
            return QIcon(":/image/optifine.png");

        //favicon
        QUrl url;
        url.setScheme(websiteUrl.scheme());
        url.setHost(hostname);
        url.setPath("/favicon.ico");
        QNetworkRequest request(url);
        QNetworkAccessManager manager;
        auto reply = manager.get(request);
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return QIcon();
        }
        auto bytes = reply->readAll();
        reply->deleteLater();
        return QIcon(bytes);
    });

    auto watcher = new QFutureWatcher<QIcon>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<QIcon>::finished, this, [=]{
        if(auto icon = watcher->result(); !icon.isNull())
            emit iconGot(icon);
        else {
            //QWebEnginePage icon
//            auto page = new QWebEnginePage(this);
//            page->load(websiteUrl);
//            connect(page, &QWebEnginePage::iconChanged, this, &WebsiteIcon::iconGot);
//            return;
        }
    });
}
