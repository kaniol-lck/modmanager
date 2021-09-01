#include "curseforgemodinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

CurseforgeModInfo::CurseforgeModInfo(int addonId) :
    id(addonId)
{}

CurseforgeModInfo CurseforgeModInfo::fromVariant(const QVariant &variant)
{
    CurseforgeModInfo modInfo;

    modInfo.basicInfo = true;
    modInfo.id = value(variant, "id").toInt();
    modInfo.name = value(variant, "name").toString();
    modInfo.summary = value(variant, "summary").toString();
    modInfo.websiteUrl = value(variant, "websiteUrl").toUrl();
    modInfo.downloadCount = value(variant, "downloadCount").toInt();

    for(const auto &str : value(variant, "modLoaders").toStringList())
        modInfo.modLoaders << ModLoaderType::fromString(str);

    //authors
    auto authorsList = value(variant, "authors").toList();
    for(const auto &author : qAsConst(authorsList))
        modInfo.authors << value(author, "name").toString();

    //thumbnail image
    auto attachmentsList = value(variant, "attachments").toList();
    if(!attachmentsList.isEmpty())
        modInfo.iconUrl = value(attachmentsList.at(0), "thumbnailUrl").toUrl();

    //latest file url
    auto latestFileList = value(variant, "latestFiles").toList();
    for(const auto &variant : latestFileList)
        modInfo.latestFileList.append(CurseforgeFileInfo::fromVariant(variant));

    return modInfo;
}

int CurseforgeModInfo::getId() const
{
    return id;
}

const QString &CurseforgeModInfo::getName() const
{
    return name;
}

const QString &CurseforgeModInfo::getSummary() const
{
    return summary;
}

const QUrl &CurseforgeModInfo::getWebsiteUrl() const
{
    return websiteUrl;
}

const QStringList &CurseforgeModInfo::getAuthors() const
{
    return authors;
}

const QUrl &CurseforgeModInfo::getThumbnailUrl() const
{
    return iconUrl;
}

const QByteArray &CurseforgeModInfo::getIconBytes() const
{
    return iconBytes;
}

const QString &CurseforgeModInfo::getDescription() const
{
    return description;
}

int CurseforgeModInfo::getDownloadCount() const
{
    return downloadCount;
}

const QList<ModLoaderType::Type> &CurseforgeModInfo::getModLoaders() const
{
    return modLoaders;
}

bool CurseforgeModInfo::isFabricMod() const
{
    return modLoaders.contains(ModLoaderType::Fabric);
}

bool CurseforgeModInfo::isForgeMod() const
{
    return modLoaders.contains(ModLoaderType::Forge);
}

bool CurseforgeModInfo::isRiftMod() const
{
    return modLoaders.contains(ModLoaderType::Rift);
}

const QList<CurseforgeFileInfo> &CurseforgeModInfo::getLatestFileList() const
{
    return latestFileList;
}

std::optional<CurseforgeFileInfo> CurseforgeModInfo::getlatestFileInfo(const GameVersion &version, ModLoaderType::Type &loaderType) const
{
    //latest last
    for(auto iter = latestFileList.rbegin(); iter < latestFileList.rend(); iter++){
        if((version == GameVersion::ANY || iter->getGameVersions().contains(version) || iter->getGameVersions().isEmpty()) &&
                (loaderType == ModLoaderType::Any || iter->getModLoaders().contains(loaderType) || iter->getModLoaders().isEmpty()))
            return {*iter};
    }
    return std::nullopt;
}

const QList<CurseforgeFileInfo> &CurseforgeModInfo::getAllFileList() const
{
    return allFileList;
}

void CurseforgeModInfo::setLatestFiles(const QList<CurseforgeFileInfo> &newLatestFiles)
{
    latestFileList = newLatestFiles;
}

bool CurseforgeModInfo::hasBasicInfo() const
{
    return basicInfo;
}
