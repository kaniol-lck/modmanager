#include "curseforgeapi.h"
#include "curseforgemodinfo.h"

#include <QDebug>

#include "util/tutil.hpp"
#include "download/assetcache.h"
#include "curseforgemodinfocaches.h"

CurseforgeModInfo::CurseforgeModInfo(int addonId) :
    CurseforgeModCacheInfo(addonId)
{
    loadIcon();
}

CurseforgeModInfo CurseforgeModInfo::fromVariant(const QVariant &variant)
{
    CurseforgeModInfo modInfo;

    modInfo.basicInfo_ = true;
    modInfo.id_ = value(variant, "id").toInt();
    modInfo.name_ = value(variant, "name").toString();
    modInfo.summary_ = value(variant, "summary").toString();
    modInfo.websiteUrl_ = value(value(variant, "links"),"websiteUrl").toUrl();
    modInfo.slug_ = value(variant,"slug").toString();
    modInfo.downloadCount_ = value(variant, "downloadCount").toInt();
    modInfo.dateModified_ = value(variant, "dateModified").toDateTime();
    modInfo.dateCreated_ = value(variant, "dateCreated").toDateTime();
    modInfo.dateReleased_ = value(variant, "dateReleased").toDateTime();
    modInfo.gamePopularityRank_ = value(variant, "gamePopularityRank").toDouble();

    //authors
    for(auto &&author : value(variant, "authors").toList())
        modInfo.authors_ << value(author, "name").toString();

    //thumbnail image
    for(auto &&attachment : value(variant, "screenshots").toList()){
        Attachment image;
        image.title = value(attachment, "title").toString();
        image.description = value(attachment, "description").toString();
        image.url = value(attachment, "url").toUrl();
        image.thumbnailUrl = value(attachment, "thumbnailUrl").toUrl();
        modInfo.images_ << image;
    }

    //icon
    modInfo.iconUrl_ = value(value(variant,"logo"),"thumbnailUrl").toUrl();


    //latest files
    for(auto &&variant : value(variant, "latestFiles").toList())
        modInfo.latestFileList_ << CurseforgeFileInfo::fromVariant(variant);

    //latest file indexes
    for(auto &&variant : value(variant, "latestFilesIndexes").toList())
        if(auto id = value(variant, "fileId").toInt(); !modInfo.latestFileIndexList_.contains(id))
            modInfo.latestFileIndexList_ << value(variant, "fileId").toInt();

    //categories
    for(auto &&variant : value(variant, "categories").toList()){
        auto category = CurseforgeCategoryInfo::fromVariant(variant);
        modInfo.categories_ << category;

        AssetCache iconAsset(nullptr, category.avatarUrl(), category.avatarUrl().fileName(), CurseforgeCategoryInfo::cachePath());
        //import as tags
        modInfo.importTag(Tag(category.name(), TagCategory::CurseforgeCategory, iconAsset.destFilePath()));
    }
    if(value(variant, "gameId").toInt() == 432)
        CurseforgeModInfoCaches::caches()->addCache(modInfo);
    return modInfo;
}

const QUrl &CurseforgeModInfo::websiteUrl() const
{
    return websiteUrl_;
}

const QStringList &CurseforgeModInfo::authors() const
{
    return authors_;
}

const QUrl &CurseforgeModInfo::iconUrl() const
{
    return iconUrl_;
}

const QString &CurseforgeModInfo::description() const
{
    return description_;
}

int CurseforgeModInfo::downloadCount() const
{
    return downloadCount_;
}

const QList<CurseforgeFileInfo> &CurseforgeModInfo::latestFiles() const
{
    return latestFileList_;
}

std::optional<CurseforgeFileInfo> CurseforgeModInfo::latestFileInfo(const GameVersion &version, ModLoaderType::Type &loaderType) const
{
    //latest last
    for(auto iter = latestFileList_.rbegin(); iter < latestFileList_.rend(); iter++){
        if((version == GameVersion::Any || iter->gameVersions().contains(version)/* || iter->gameVersions().isEmpty()*/) &&
                (loaderType == ModLoaderType::Any || iter->loaderTypes().contains(loaderType)/* || iter->loaderTypes().isEmpty()*/))
            return {*iter};
    }
    return std::nullopt;
}

const QList<CurseforgeFileInfo> &CurseforgeModInfo::allFileList() const
{
    return allFileList_;
}

void CurseforgeModInfo::setLatestFiles(const QList<CurseforgeFileInfo> &newLatestFiles)
{
    latestFileList_ = newLatestFiles;
}

bool CurseforgeModInfo::hasBasicInfo() const
{
    return basicInfo_;
}

bool CurseforgeModInfo::fileCompleted() const
{
    return allFileList_.size() != 0 && allFileList_.size() == totalFileCount_;
}

int CurseforgeModInfo::totalFileCount() const
{
    return totalFileCount_;
}

void CurseforgeModInfo::setTotalFileCount(int newTotalFileCount)
{
    totalFileCount_ = newTotalFileCount;
}

const QList<int> &CurseforgeModInfo::latestFileIndexList() const
{
    return latestFileIndexList_;
}

const QList<CurseforgeFileInfo> &CurseforgeModInfo::latestIndexedFileList() const
{
    return latestIndexedFileList_;
}

const QList<CurseforgeCategoryInfo> &CurseforgeModInfo::categories() const
{
    return categories_;
}

const QList<CurseforgeModInfo::Attachment> &CurseforgeModInfo::images() const
{
    return images_;
}

const QDateTime &CurseforgeModInfo::dateModified() const
{
    return dateModified_;
}

const QDateTime &CurseforgeModInfo::dateCreated() const
{
    return dateCreated_;
}

const QDateTime &CurseforgeModInfo::dateReleased() const
{
    return dateReleased_;
}
