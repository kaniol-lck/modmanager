#include "curseforgefileinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

CurseforgeFileInfo::CurseforgeFileInfo()
{

}

CurseforgeFileInfo CurseforgeFileInfo::fromVariant(const QVariant &variant)
{
    CurseforgeFileInfo curseforgeModFileInfo;

    curseforgeModFileInfo.id = value(variant, "id").toInt();
    curseforgeModFileInfo.displayName = value(variant, "displayName").toString();
    curseforgeModFileInfo.fileName = value(variant, "fileName").toString();
    curseforgeModFileInfo.downloadUrl = value(variant, "downloadUrl").toUrl();
    curseforgeModFileInfo.fileLength = value(variant, "fileLength").toInt();

    //I don't know why curseforge put game verison and modloader together
    auto versionList = value(variant, "gameVersion").toStringList();
    for(const auto &version : versionList){
        auto v = GameVersion::deduceFromString(version);
        if(v.has_value()){
            curseforgeModFileInfo.gameVersions << v.value();
            continue;
        }
        auto loaderType = ModLoaderType::fromString(version);
        if(loaderType != ModLoaderType::Any){
            curseforgeModFileInfo.modLoaders << loaderType;
            continue;
        }
    }

    return curseforgeModFileInfo;
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
