#include "modrinthfileinfo.h"

#include "util/tutil.hpp"

ModrinthFileInfo ModrinthFileInfo::fromVariant(const QVariant &variant)
{

    ModrinthFileInfo modrinthFileInfo;

    modrinthFileInfo.id = value(variant, "id").toString();
    modrinthFileInfo.modId = value(variant, "mod_id").toString();
    modrinthFileInfo.displayName = value(variant, "name").toString();
    modrinthFileInfo.releaseType = value(variant, "version_type").toInt();
    modrinthFileInfo.fileDate = value(variant, "date_published").toDateTime();

    auto files = value(variant, "files").toList();

    if(!files.isEmpty()){
        modrinthFileInfo.fileName = value(files.at(0), "filename").toString();
        modrinthFileInfo.downloadUrl = value(files.at(0), "url").toUrl();
    }

    auto versionList = value(variant, "game_versions").toStringList();
    for(const auto &version : versionList)
        modrinthFileInfo.gameVersions << GameVersion(version);

    auto loaderList = value(variant, "loaders").toStringList();
    for(const auto &loader : loaderList)
        modrinthFileInfo.modLoaders << ModLoaderType::fromString(loader);

    return modrinthFileInfo;
}

const QString &ModrinthFileInfo::getId() const
{
    return id;
}

const QString &ModrinthFileInfo::getModId() const
{
    return modId;
}

const QString &ModrinthFileInfo::getDisplayName() const
{
    return displayName;
}

const QString &ModrinthFileInfo::getFileName() const
{
    return fileName;
}

const QUrl &ModrinthFileInfo::getDownloadUrl() const
{
    return downloadUrl;
}

const QList<GameVersion> &ModrinthFileInfo::getGameVersions() const
{
    return gameVersions;
}

const QList<ModLoaderType::Type> &ModrinthFileInfo::getModLoaders() const
{
    return modLoaders;
}

int ModrinthFileInfo::getFileLength() const
{
    return fileLength;
}

int ModrinthFileInfo::getReleaseType() const
{
    return releaseType;
}

const QDateTime &ModrinthFileInfo::getFileDate() const
{
    return fileDate;
}
