#include "curseforgemodinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

CurseforgeModInfo::CurseforgeModInfo(int addonId) :
    id_(addonId)
{}

CurseforgeModInfo CurseforgeModInfo::fromVariant(const QVariant &variant)
{
    CurseforgeModInfo modInfo;

    modInfo.basicInfo_ = true;
    modInfo.id_ = value(variant, "id").toInt();
    modInfo.name_ = value(variant, "name").toString();
    modInfo.summary_ = value(variant, "summary").toString();
    modInfo.websiteUrl_ = value(variant, "websiteUrl").toUrl();
    modInfo.downloadCount_ = value(variant, "downloadCount").toInt();

    for(const auto &str : value(variant, "modLoaders").toStringList())
        modInfo.loaderTypes_ << ModLoaderType::fromString(str);

    //authors
    auto authorsList = value(variant, "authors").toList();
    for(const auto &author : qAsConst(authorsList))
        modInfo.authors_ << value(author, "name").toString();

    //thumbnail image
    auto attachmentsList = value(variant, "attachments").toList();
    if(!attachmentsList.isEmpty())
        modInfo.iconUrl_ = value(attachmentsList.at(0), "thumbnailUrl").toUrl();

    //latest file url
    auto latestFileList = value(variant, "latestFiles").toList();
    for(const auto &variant : latestFileList)
        modInfo.latestFileList_.append(CurseforgeFileInfo::fromVariant(variant));

    return modInfo;
}

int CurseforgeModInfo::id() const
{
    return id_;
}

const QString &CurseforgeModInfo::name() const
{
    return name_;
}

const QString &CurseforgeModInfo::summary() const
{
    return summary_;
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

const QByteArray &CurseforgeModInfo::iconBytes() const
{
    return iconBytes_;
}

const QString &CurseforgeModInfo::description() const
{
    return description_;
}

int CurseforgeModInfo::downloadCount() const
{
    return downloadCount_;
}

const QList<ModLoaderType::Type> &CurseforgeModInfo::loaderTypes() const
{
    return loaderTypes_;
}

bool CurseforgeModInfo::isFabricMod() const
{
    return loaderTypes_.contains(ModLoaderType::Fabric);
}

bool CurseforgeModInfo::isForgeMod() const
{
    return loaderTypes_.contains(ModLoaderType::Forge);
}

bool CurseforgeModInfo::isRiftMod() const
{
    return loaderTypes_.contains(ModLoaderType::Rift);
}

const QList<CurseforgeFileInfo> &CurseforgeModInfo::latestFileList() const
{
    return latestFileList_;
}

std::optional<CurseforgeFileInfo> CurseforgeModInfo::latestFileInfo(const GameVersion &version, ModLoaderType::Type &loaderType) const
{
    //latest last
    for(auto iter = latestFileList_.rbegin(); iter < latestFileList_.rend(); iter++){
        if((version == GameVersion::ANY || iter->gameVersions().contains(version)/* || iter->gameVersions().isEmpty()*/) &&
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
