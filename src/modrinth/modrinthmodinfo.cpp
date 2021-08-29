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
    modrinthModInfo.versions = value(variant, "versions").toStringList();
    modrinthModInfo.downloadCount = value(variant, "downloads").toInt();
    modrinthModInfo.iconUrl = value(variant, "icon_url").toUrl();

    return modrinthModInfo;
}

ModrinthModInfo ModrinthModInfo::fromVariant(const QVariant &variant)
{
    ModrinthModInfo modrinthModInfo;
    modrinthModInfo.basicInfo = true;
    modrinthModInfo.fullInfo = true;

    modrinthModInfo.id = value(variant, "id").toString();
    modrinthModInfo.name = value(variant, "title").toString();
    modrinthModInfo.author = value(variant, "author").toString();
    modrinthModInfo.summary = value(variant, "description").toString();
    modrinthModInfo.description = value(variant, "body").toString();
//    modrinthModInfo.versions = value(variant, "versions").toStringList();
    modrinthModInfo.downloadCount = value(variant, "downloads").toInt();
    modrinthModInfo.iconUrl = value(variant, "icon_url").toUrl();

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

const QStringList &ModrinthModInfo::getVersions() const
{
    return versions;
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
