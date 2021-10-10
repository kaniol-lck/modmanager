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

void CurseforgeMod::acquireBasicInfo()
{
    if(gettingBasicInfo_) return;
    gettingBasicInfo_ = true;
    api_->getInfo(modInfo_.id_, [=](const auto &info){
        gettingBasicInfo_ = false;
        modInfo_ = info;
        emit basicInfoReady();
        modInfo_.basicInfo_ = true;
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
    api_->getDescription(modInfo_.id(), [=](const QString &description){
        gettingDescription_ = false;
        modInfo_.description_ = description;
        emit descriptionReady();
    });
}

void CurseforgeMod::acquireAllFileList()
{
    if(gettingAllFileList_) return;
    gettingAllFileList_ = true;
    api_->getFiles(modInfo_.id(), [=](const QList<CurseforgeFileInfo> &fileList){
        gettingAllFileList_ = false;
        modInfo_.allFileList_ = fileList;
        emit allFileListReady();
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
            auto [id, name, iconName] = *it;
            tags << Tag(name, TagCategory::CurseforgeCategory, ":/image/curseforge/" + iconName);
        }
    }
    return tags;
}
