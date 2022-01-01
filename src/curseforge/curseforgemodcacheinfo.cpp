#include "curseforgemodcacheinfo.h"

#include <QDir>
#include <QJsonObject>
#include <QStandardPaths>

#include "download/assetcache.h"
#include "util/tutil.hpp"

CurseforgeModCacheInfo::CurseforgeModCacheInfo(int addonId) :
    id_(addonId)
{}

CurseforgeModCacheInfo CurseforgeModCacheInfo::fromVariant(const QVariant &variant)
{
    CurseforgeModCacheInfo info;
    info.id_ = value(variant, "id").toInt();
    info.name_ = value(variant, "name").toString();
    info.summary_ = value(variant, "summary").toString();
    if(variant.toMap().contains("slug"))
        info.slug_ = value(variant, "slug").toString();
    info.iconUrl_ = value(variant, "iconUrl").toUrl();
    AssetCache iconCache(nullptr, info.iconUrl_, info.iconUrl_.fileName(), cachePath());
    if(iconCache.exists())
        info.icon_.load(iconCache.destFilePath());
    info.popularityScore_ = value(variant, "popularityScore").toDouble();
    return info;
}

QJsonObject CurseforgeModCacheInfo::toJsonObject() const
{
    QJsonObject object;
    object.insert("id", id_);
    object.insert("name", name_);
    object.insert("summary", summary_);
    object.insert("slug", slug_);
    object.insert("iconUrl", iconUrl_.toString());
    object.insert("popularityScore", popularityScore_);
    return object;
}

bool CurseforgeModCacheInfo::operator==(const CurseforgeModCacheInfo &other) const
{
    return toJsonObject() == other.toJsonObject();
}

const QString &CurseforgeModCacheInfo::cachePath()
{
    static const QString path =
            QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
            .absoluteFilePath("curseforge/mods");
    return path;
}

int CurseforgeModCacheInfo::id() const
{
    return id_;
}

const QString &CurseforgeModCacheInfo::name() const
{
    return name_;
}

const QString &CurseforgeModCacheInfo::summary() const
{
    return summary_;
}

const QString &CurseforgeModCacheInfo::slug() const
{
    return slug_;
}

const QUrl &CurseforgeModCacheInfo::iconUrl() const
{
    return iconUrl_;
}

const QPixmap &CurseforgeModCacheInfo::icon() const
{
    return icon_;
}

double CurseforgeModCacheInfo::popularityScore() const
{
    return popularityScore_;
}
