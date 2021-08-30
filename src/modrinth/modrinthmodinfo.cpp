#include "modrinthmodinfo.h"

#include "util/tutil.hpp"

ModrinthModInfo ModrinthModInfo::fromSearchVariant(const QVariant &variant)
{
    ModrinthModInfo modInfo;
    modInfo.basicInfo = true;

    modInfo.id = value(variant, "mod_id").toString();
    modInfo.name = value(variant, "title").toString();
    modInfo.author = value(variant, "author").toString();
    modInfo.summary = value(variant, "description").toString();
    modInfo.downloadCount = value(variant, "downloads").toInt();
    modInfo.iconUrl = value(variant, "icon_url").toUrl();
    modInfo.websiteUrl = value(variant, "page_url").toUrl();

    for(const auto &v : value(variant, "versions").toList())
        modInfo.gameVersions << GameVersion(v.toString());

    return modInfo;
}

ModrinthModInfo ModrinthModInfo::fromVariant(const QVariant &variant)
{
    ModrinthModInfo modInfo;
    modInfo.basicInfo = true;
    modInfo.fullInfo = true;

    modInfo.id = value(variant, "id").toString();
    modInfo.name = value(variant, "title").toString();
    modInfo.summary = value(variant, "description").toString();
    modInfo.description = value(variant, "body").toString();
    modInfo.downloadCount = value(variant, "downloads").toInt();
    modInfo.iconUrl = value(variant, "icon_url").toUrl();
    modInfo.versionList = value(variant, "versions").toStringList();

    return modInfo;
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
