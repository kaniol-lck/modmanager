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
    api_(parent->modrinthAPI()),
    modInfo_(id)
{}

ModrinthMod::ModrinthMod(QObject *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(ModrinthAPI::api()),
    modInfo_(info)
{}

ModrinthMod::ModrinthMod(LocalMod *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(parent->modrinthAPI()),
    modInfo_(info)
{}

ModrinthMod::~ModrinthMod()
{
    MMLogger::dector(this) << modInfo_.id() << modInfo_.name();
}

ModrinthModInfo ModrinthMod::modInfo() const
{
    return modInfo_;
}

QList<Tag> ModrinthMod::tags() const
{
    QList<Tag> tags;
    for(auto &&categoryId : modInfo_.categories()){
        auto it = std::find_if(ModrinthAPI::getCategories().cbegin(), ModrinthAPI::getCategories().cend(), [=](auto &&t){
            return std::get<1>(t) == categoryId;
        });
        if(it != ModrinthAPI::getCategories().end()){
            auto [name, iconName] = *it;
            tags << Tag(name, TagCategory::ModrinthCategory, ":/image/modrinth/" + iconName);
        }
    }
    return tags;
}

void ModrinthMod::acquireAuthor()
{
    if(modInfo_.authorId_.isEmpty() || gettingAuthor_) return;
    gettingAuthor_ = true;
    api_->getAuthor(modInfo_.modId_, [=](const auto &author){
        gettingAuthor_ = false;
        modInfo_.author_ = author;
        emit authorReady();
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
    api_->getInfo(modInfo_.modId_, [=](const auto &newInfo){
        gettingFullInfo_ = false;
        if(modInfo_.basicInfo_){
            modInfo_.description_ = newInfo.description_;
            modInfo_.versionList_ = newInfo.versionList_;
        } else
            modInfo_ = newInfo;
        emit fullInfoReady();
    });
}

void ModrinthMod::acquireFileList()
{
    if(gettingFileList_) return;
    gettingFileList_ = true;

    api_->getVersions(modInfo_.modId_, [=](const auto &files){
        gettingFileList_ = false;
        modInfo_.fileList_ = files;
        emit fileListReady();
    });
}
