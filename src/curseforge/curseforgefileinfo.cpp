#include "curseforgefileinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

CurseforgeFileInfo CurseforgeFileInfo::fromVariant(const QVariant &variant)
{
    CurseforgeFileInfo curseforgeFileInfo;

    curseforgeFileInfo.id = value(variant, "id").toInt();
    curseforgeFileInfo.displayName = value(variant, "displayName").toString();
    curseforgeFileInfo.fileName = value(variant, "fileName").toString();
    curseforgeFileInfo.downloadUrl = value(variant, "downloadUrl").toUrl();
    curseforgeFileInfo.fileLength = value(variant, "fileLength").toInt();
    curseforgeFileInfo.releaseType = value(variant, "releaseType").toInt();
    curseforgeFileInfo.fileDate = value(variant, "fileDate").toDateTime();

    //I don't know why curseforge put game verison and modloader together
    auto versionList = value(variant, "gameVersion").toStringList();
    for(const auto &version : versionList){
        auto v = GameVersion::deduceFromString(version);
        if(v.has_value()){
            curseforgeFileInfo.gameVersions << v.value();
            continue;
        }
        auto loaderType = ModLoaderType::fromString(version);
        if(loaderType != ModLoaderType::Any){
            curseforgeFileInfo.modLoaders << loaderType;
            continue;
        }
    }

    return curseforgeFileInfo;
}

int CurseforgeFileInfo::getId() const
{
    return id;
}

const QString &CurseforgeFileInfo::getDisplayName() const
{
    return displayName;
}

const QUrl &CurseforgeFileInfo::getDownloadUrl() const
{
    return downloadUrl;
}

const QList<GameVersion> &CurseforgeFileInfo::getGameVersions() const
{
    return gameVersions;
}

const QList<ModLoaderType::Type> &CurseforgeFileInfo::getModLoaders() const
{
    return modLoaders;
}

int CurseforgeFileInfo::getFileLength() const
{
    return fileLength;
}

const QString &CurseforgeFileInfo::getFileName() const
{
    return fileName;
}

int CurseforgeFileInfo::getReleaseType() const
{
    return releaseType;
}

const QDateTime &CurseforgeFileInfo::getFileDate() const
{
    return fileDate;
}
