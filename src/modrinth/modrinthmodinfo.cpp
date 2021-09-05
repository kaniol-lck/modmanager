#include "modrinthmodinfo.h"

#include "util/tutil.hpp"

ModrinthModInfo::ModrinthModInfo(const QString &id) :
    modId_(id)
{}

ModrinthModInfo ModrinthModInfo::fromSearchVariant(const QVariant &variant)
{
    ModrinthModInfo modInfo;
    modInfo.basicInfo_ = true;

    modInfo.modId_ = value(variant, "mod_id").toString();
    modInfo.name_ = value(variant, "title").toString();
    modInfo.author_ = value(variant, "author").toString();
    modInfo.summary_ = value(variant, "description").toString();
    modInfo.downloadCount_ = value(variant, "downloads").toInt();
    modInfo.iconUrl_ = value(variant, "icon_url").toUrl();
    modInfo.websiteUrl_ = value(variant, "page_url").toUrl();

    for(const auto &v : value(variant, "versions").toList())
        modInfo.gameVersions_ << GameVersion(v.toString());

    return modInfo;
}

ModrinthModInfo ModrinthModInfo::fromVariant(const QVariant &variant)
{
    ModrinthModInfo modInfo;
    //full, but no author and website url :C
    modInfo.basicInfo_ = true;
    modInfo.fullInfo_ = true;

    modInfo.modId_ = value(variant, "id").toString();
    modInfo.name_ = value(variant, "title").toString();
    modInfo.summary_ = value(variant, "description").toString();
    modInfo.description_ = value(variant, "body").toString();
    modInfo.downloadCount_ = value(variant, "downloads").toInt();
    modInfo.iconUrl_ = value(variant, "icon_url").toUrl();
    modInfo.versionList_ = value(variant, "versions").toStringList();

    return modInfo;
}

const QString &ModrinthModInfo::id() const
{
    return modId_;
}

const QString &ModrinthModInfo::author() const
{
    return author_;
}

const QString &ModrinthModInfo::name() const
{
    return name_;
}

const QString &ModrinthModInfo::description() const
{
    return description_;
}

int ModrinthModInfo::downloadCount() const
{
    return downloadCount_;
}

const QUrl &ModrinthModInfo::iconUrl() const
{
    return iconUrl_;
}

const QByteArray &ModrinthModInfo::iconBytes() const
{
    return iconBytes_;
}

const QString &ModrinthModInfo::summary() const
{
    return summary_;
}

const QUrl &ModrinthModInfo::websiteUrl() const
{
    return websiteUrl_;
}

const QList<ModrinthFileInfo> &ModrinthModInfo::fileList() const
{
    return fileList_;
}

const QList<GameVersion> &ModrinthModInfo::gameVersions() const
{
    return gameVersions_;
}

const QStringList &ModrinthModInfo::versionList() const
{
    return versionList_;
}

bool ModrinthModInfo::hasBasicInfo() const
{
    return basicInfo_;
}

bool ModrinthModInfo::hasFullInfo() const
{
    return fullInfo_;
}
