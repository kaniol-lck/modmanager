#include "curseforgemod.h"

#include <QStandardPaths>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "local/localmod.h"
#include "curseforge/curseforgeapi.h"
#include "util/tutil.hpp"
#include "util/funcutil.h"
#include "util/mmlogger.h"

CurseforgeMod::CurseforgeMod(QObject *parent, int id) :
    QObject(parent),
    api_(CurseforgeAPI::api()),
    modInfo_(id)
{}

CurseforgeMod::CurseforgeMod(LocalMod *parent, int id) :
    QObject(parent),
    api_(parent->curseforgeAPI()),
    modInfo_(id)
{}

CurseforgeMod::CurseforgeMod(QObject *parent, const CurseforgeModInfo &info) :
    QObject(parent),
    api_(CurseforgeAPI::api()),
    modInfo_(info)
{}

CurseforgeMod::CurseforgeMod(LocalMod *parent, const CurseforgeModInfo &info) :
    QObject(parent),
    api_(parent->curseforgeAPI()),
    modInfo_(info)
{}

CurseforgeMod::~CurseforgeMod()
{
    MMLogger::dtor(this) << modInfo_.id() << modInfo_.name();
}

void CurseforgeMod::acquireBasicInfo()
{
    if(gettingBasicInfo_) return;
    gettingBasicInfo_ = true;
    auto conn = api_->getInfo(modInfo_.id_, [=](const auto &info){
        gettingBasicInfo_ = false;
        modInfo_ = info;
        emit basicInfoReady();
        modInfo_.basicInfo_ = true;
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

void CurseforgeMod::acquireIcon()
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

void CurseforgeMod::acquireDescription()
{
    if(gettingDescription_) return;
    gettingDescription_ = true;
    auto conn = api_->getDescription(modInfo_.id(), [=](const QString &description){
        gettingDescription_ = false;
        modInfo_.description_ = description;
        emit descriptionReady();
    });
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

void CurseforgeMod::acquireAllFileList(std::function<void (QList<CurseforgeFileInfo>)> callback, std::function<void ()> failed)
{
    if(gettingAllFileList_) return;
    gettingAllFileList_ = true;
    auto conn = api_->getFiles(modInfo_.id(), [=](const QList<CurseforgeFileInfo> &fileList){
        gettingAllFileList_ = false;
        modInfo_.allFileList_ = fileList;
        callback(fileList);
        emit allFileListReady();
    }, failed);
    connect(this, &QObject::destroyed, this, [=]{
        disconnect(conn);
    });
}

const CurseforgeModInfo &CurseforgeMod::modInfo() const
{
    return modInfo_;
}

QList<Tag> CurseforgeMod::tags() const
{
    QList<Tag> tags;
    for(auto &&categoryId : modInfo_.categories()){
        auto it = std::find_if(CurseforgeAPI::getCategories().cbegin(), CurseforgeAPI::getCategories().cend(), [=](auto &&t){
            return std::get<0>(t) == categoryId;
        });
        if(it != CurseforgeAPI::getCategories().end()){
            auto [id, name, iconName, parentId] = *it;
            tags << Tag(name, TagCategory::CurseforgeCategory, ":/image/curseforge/" + iconName);
        } else
            qDebug() << categoryId;
    }
    return tags;
}
