#include "assetcache.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "config.hpp"

namespace {

//所有图片共用一个 QNetworkAccessManager：它自带同主机 6 连接的上限，
//并发请求会自动排队，不会无限开连接。
QNetworkAccessManager *sharedAccessManager()
{
    static auto *manager = []{
        auto *m = new QNetworkAccessManager;
        //与项目其它网络代码保持一致。不设超时的话，连接卡住时这个 reply 永远不会
        //finished，既泄漏资源，也让调用方的「正在获取」标记永远复位不了。
        m->setTransferTimeout(Config().getNetworkRequestTimeout());
        return m;
    }();
    return manager;
}

//同一个目标文件可能被多个对象几乎同时请求（例如列表与详情弹窗同时显示同一个 mod）。
//并发下载会同时写同一个文件、互相截断，所以让后来者挂到已在进行的那次下载上。
QHash<QString, AssetCache *> &inFlightDownloads()
{
    static QHash<QString, AssetCache *> map;
    return map;
}

} // namespace

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
    //已有同目标文件的下载在进行：复用它，等它出结果即可，不再发第二次请求
    if(auto it = inFlightDownloads().find(destFilePath_);
            it != inFlightDownloads().end() && it.value() && it.value() != this){
        auto *pending = it.value();
        connect(pending, &AssetCache::assetReady, this, &AssetCache::assetReady);
        connect(pending, &AssetCache::assetFailed, this, &AssetCache::assetFailed);
        return;
    }

    QNetworkRequest request(url_);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    auto reply = sharedAccessManager()->get(request);
    inFlightDownloads().insert(destFilePath_, this);

    connect(reply, &QNetworkReply::redirected, this, [=](QUrl url){
        qDebug() << "redirected: " << url_ << " -> " << url;
    });
    connect(reply, &QNetworkReply::finished, this, [=]{
        //无论成败都必须回收 reply。原实现把 deleteLater() 写在 error 分支的 return 之后，
        //导致每次下载失败都泄漏一个 QNetworkReply。
        if(reply->error() == QNetworkReply::NoError){
            QFile file(destFilePath_);
            QDir().mkpath(QFileInfo(destFilePath_).absolutePath());
            if(file.open(QIODevice::WriteOnly)){
                file.write(reply->readAll());
                file.close();
                emit assetReady();
            } else
                emit assetFailed();
        } else{
            qDebug() << "asset download failed:" << url_ << reply->error();
            emit assetFailed();
        }
        if(inFlightDownloads().value(destFilePath_) == this)
            inFlightDownloads().remove(destFilePath_);
        reply->deleteLater();
    });
    //AssetCache 先于下载结束被销毁时，清掉在途登记，避免留下悬空指针。
    //按值捕获路径与自身地址——destroyed 是在 ~QObject 里发的，那时派生类成员已经析构了。
    const QString path = destFilePath_;
    const AssetCache *self = this;
    connect(this, &QObject::destroyed, [path, self]{
        if(inFlightDownloads().value(path) == self)
            inFlightDownloads().remove(path);
    });
}

const QString &AssetCache::destFilePath() const
{
    return destFilePath_;
}
