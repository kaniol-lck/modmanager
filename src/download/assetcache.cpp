#include "assetcache.h"

#include <QDir>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

AssetCache::AssetCache(QObject *parent, const QUrl &url, const QString &destFilePath) :
    QObject(parent),
    url_(url),
    destFilePath_(destFilePath)
{}

AssetCache::AssetCache(QObject *parent, const QUrl &url, const QString &fileName, const QString &path) :
    AssetCache(parent, url, QDir(path).absoluteFilePath(fileName))
{}

bool AssetCache::exists() const
{
    return QFileInfo::exists(destFilePath_);
}

void AssetCache::download()
{
    QNetworkRequest request(url_);
    static QNetworkAccessManager accessManager;
    auto reply = accessManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) return;
        QFile file(destFilePath_);
        QDir().mkpath(QFileInfo(destFilePath_).absolutePath());
        if(file.open(QIODevice::WriteOnly)){
            file.write(reply->readAll());
            emit assetReady();
        }
        reply->deleteLater();
    });
}

const QString &AssetCache::destFilePath() const
{
    return destFilePath_;
}
