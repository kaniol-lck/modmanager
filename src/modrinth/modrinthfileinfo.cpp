#include "modrinthfileinfo.h"

#include "util/tutil.hpp"

ModrinthFileInfo ModrinthFileInfo::fromVariant(const QVariant &variant)
{

    ModrinthFileInfo fileInfo;

    fileInfo.id_ = value(variant, "id").toString();
    fileInfo.modId_ = value(variant, "mod_id").toString();
    fileInfo.displayName_ = value(variant, "name").toString();
    fileInfo.releaseType_ = value(variant, "version_type").toInt();
    fileInfo.fileDate_ = value(variant, "date_published").toDateTime();

    auto files = value(variant, "files").toList();

    if(!files.isEmpty()){
        fileInfo.fileName_ = value(files.at(0), "filename").toString();
        fileInfo.url_ = value(files.at(0), "url").toUrl();
    }

    auto versionList = value(variant, "game_versions").toStringList();
    for(const auto &version : versionList)
        fileInfo.gameVersions_ << GameVersion(version);

    auto loaderList = value(variant, "loaders").toStringList();
    for(const auto &loader : loaderList)
        fileInfo.loaderTypes_ << ModLoaderType::fromString(loader);

    return fileInfo;
}

DownloadFileInfo::SourceType ModrinthFileInfo::source() const
{
    return DownloadFileInfo::Modrinth;
}

const QString &ModrinthFileInfo::id() const
{
    return id_;
}

const QString &ModrinthFileInfo::modId() const
{
    return modId_;
}

const QList<GameVersion> &ModrinthFileInfo::gameVersions() const
{
    return gameVersions_;
}

const QList<ModLoaderType::Type> &ModrinthFileInfo::loaderTypes() const
{
    return loaderTypes_;
}

int ModrinthFileInfo::size() const
{
    return size_;
}

int ModrinthFileInfo::releaseType() const
{
    return releaseType_;
}

const QDateTime &ModrinthFileInfo::fileDate() const
{
    return fileDate_;
}
