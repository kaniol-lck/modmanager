#include "modrinthmod.h"

#include <QStandardPaths>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>

#include "modrinthapi.h"
#include "local/localmod.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"

#include "util/mmlogger.h"

ModrinthMod::ModrinthMod(LocalMod *parent, const QString &id) :
    QObject(parent),
    api_(parent->modrinthAPI())
{
    setModInfo(ModrinthModInfo(id));
}

ModrinthMod::ModrinthMod(QObject *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(ModrinthAPI::api())
{
    setModInfo(info);
}

ModrinthMod::ModrinthMod(LocalMod *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(parent->modrinthAPI())
{
    setModInfo(info);
}

ModrinthMod::~ModrinthMod()
{
    MMLogger::dtor(this) << modInfo_.id() << modInfo_.name();
}

ModrinthModInfo ModrinthMod::modInfo() const
{
    return modInfo_;
}

void ModrinthMod::acquireAuthor()
{
    if(modInfo_.authorId_.isEmpty() || gettingAuthor_) return;
    gettingAuthor_ = true;
    auto conn = api_->getAuthor(modInfo_.modId_, [=](const auto &author){
        gettingAuthor_ = false;
        modInfo_.author_ = author;
        emit authorReady();
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

void ModrinthMod::acquireIcon()
{
    if(modInfo_.iconUrl_.isEmpty() || gettingIcon_) return;
    gettingIcon_ = true;
    QNetworkRequest request(modInfo_.iconUrl_);
    static QNetworkAccessManager accessManager;
    static QNetworkDiskCache diskCache;
    diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    accessManager.setCache(&diskCache);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    auto reply = accessManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        gettingIcon_ = false;
        if(reply->error() != QNetworkReply::NoError) return;
        modInfo_.iconBytes_ = reply->readAll();
        if(!modInfo_.iconBytes_.isEmpty())
            emit iconReady();
        reply->deleteLater();
    });
}

void ModrinthMod::acquireFullInfo()
{
    if(gettingFullInfo_) return;
    gettingFullInfo_ = true;
    auto conn = api_->getInfo(modInfo_.modId_, [=](const auto &newInfo){
        gettingFullInfo_ = false;
        if(modInfo_.basicInfo_){
            modInfo_.description_ = newInfo.description_;
            modInfo_.versionList_ = newInfo.versionList_;
        } else
            modInfo_ = newInfo;
        emit fullInfoReady();
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

QMetaObject::Connection ModrinthMod::acquireFileList()
{
    if(gettingFileList_) return {};
    gettingFileList_ = true;

    auto conn = api_->getVersions(modInfo_.modId_, [=](const auto &files){
        gettingFileList_ = false;
        modInfo_.fileList_ = files;
        emit fileListReady(files);
    }, [=]{
        emit fileListReady({});
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
    return conn;
}

void ModrinthMod::setModInfo(ModrinthModInfo newModInfo)
{
    modInfo_ = newModInfo;
    addSubTagable(&modInfo_);
}
