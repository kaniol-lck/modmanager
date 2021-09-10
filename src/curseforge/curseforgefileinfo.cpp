#include "curseforgefileinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

CurseforgeFileInfo CurseforgeFileInfo::fromVariant(const QVariant &variant)
{
    CurseforgeFileInfo fileInfo;

    fileInfo.id_ = value(variant, "id").toInt();
    fileInfo.displayName_ = value(variant, "displayName").toString();
    fileInfo.fileName_ = value(variant, "fileName").toString();
    fileInfo.url_ = value(variant, "downloadUrl").toUrl();
    fileInfo.size_ = value(variant, "fileLength").toInt();
    fileInfo.releaseType_ = value(variant, "releaseType").toInt();
    fileInfo.fileDate_ = value(variant, "fileDate").toDateTime();

    //I don't know why curseforge put game verison and modloader together
    auto versionList = value(variant, "gameVersion").toStringList();
    for(const auto &version : versionList){
        auto loaderType = ModLoaderType::fromString(version);
        if(loaderType != ModLoaderType::Any)
            fileInfo.loaderTypes_ << loaderType;
        else {
            auto v = GameVersion::deduceFromString(version);
            if(v.has_value())
                fileInfo.gameVersions_ << v.value();
            else
                fileInfo.gameVersions_ << version;
        }
    }

    return fileInfo;
}

DownloadFileInfo::SourceType CurseforgeFileInfo::source() const
{
    return Curseforge;
}

int CurseforgeFileInfo::getId() const
{
    return id_;
}

const QList<GameVersion> &CurseforgeFileInfo::gameVersions() const
{
    return gameVersions_;
}

const QList<ModLoaderType::Type> &CurseforgeFileInfo::loaderTypes() const
{
    return loaderTypes_;
}

int CurseforgeFileInfo::releaseType() const
{
    return releaseType_;
}

const QDateTime &CurseforgeFileInfo::fileDate() const
{
    return fileDate_;
}
