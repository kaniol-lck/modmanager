#include "modrinthmodinfo.h"

#include "util/tutil.hpp"

ModrinthModInfo ModrinthModInfo::fromSearchVariant(const QVariant &variant)
{
    ModrinthModInfo modrinthModInfo;
    modrinthModInfo.basicInfo = true;

    modrinthModInfo.id = value(variant, "mod_id").toString();
    modrinthModInfo.name = value(variant, "title").toString();
    modrinthModInfo.author = value(variant, "author").toString();
    modrinthModInfo.summary = value(variant, "description").toString();
    modrinthModInfo.downloadCount = value(variant, "downloads").toInt();
    modrinthModInfo.iconUrl = value(variant, "icon_url").toUrl();
    modrinthModInfo.websiteUrl = value(variant, "page_url").toUrl();

    for(const auto &v : value(variant, "versions").toList())
        modrinthModInfo.gameVersions << GameVersion(v.toString());

    return modrinthModInfo;
}

ModrinthModInfo ModrinthModInfo::fromVariant(const QVariant &variant)
{
    ModrinthModInfo modrinthModInfo;
    modrinthModInfo.basicInfo = true;
    modrinthModInfo.fullInfo = true;

    modrinthModInfo.id = value(variant, "id").toString();
    modrinthModInfo.name = value(variant, "title").toString();
//    modrinthModInfo.author = value(variant, "author").toString();
    modrinthModInfo.summary = value(variant, "description").toString();
    modrinthModInfo.description = value(variant, "body").toString();
    modrinthModInfo.downloadCount = value(variant, "downloads").toInt();
    modrinthModInfo.iconUrl = value(variant, "icon_url").toUrl();
    modrinthModInfo.versionList = value(variant, "versions").toStringList();

    return modrinthModInfo;
}

const QString &ModrinthModInfo::getId() const
{
    return id;
}

const QString &ModrinthModInfo::getAuthor() const
{
    return author;
}

const QString &ModrinthModInfo::getName() const
{
    return name;
}

const QString &ModrinthModInfo::getDescription() const
{
    return description;
}

int ModrinthModInfo::getDownloadCount() const
{
    return downloadCount;
}

const QUrl &ModrinthModInfo::getIconUrl() const
{
    return iconUrl;
}

const QByteArray &ModrinthModInfo::getIconBytes() const
{
    return iconBytes;
}

const QString &ModrinthModInfo::getSummary() const
{
    return summary;
}

const QUrl &ModrinthModInfo::getWebsiteUrl() const
{
    return websiteUrl;
}

const QList<ModrinthFileInfo> &ModrinthModInfo::getFileList() const
{
    return fileList;
}

const QList<GameVersion> &ModrinthModInfo::getGameVersions() const
{
    return gameVersions;
}

const QStringList &ModrinthModInfo::getVersionList() const
{
    return versionList;
}
