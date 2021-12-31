#include "curseforgemodinfocaches.h"

#include "util/tutil.hpp"

CurseforgeModInfoCaches::CurseforgeModInfoCache CurseforgeModInfoCaches::CurseforgeModInfoCache::fromVariant(const QVariant &variant)
{
    CurseforgeModInfoCache info;
    info.id_ = value(variant, "id").toInt();
    info.name_ = value(variant, "name").toString();
    info.summary_ = value(variant, "summary").toString();
    info.slug_ = value(variant, "slug").toString();
    info.popularityScore_ = value(variant, "popularityScore").toDouble();
    return info;
}

CurseforgeModInfoCaches::CurseforgeModInfoCache CurseforgeModInfoCaches::CurseforgeModInfoCache::fromInfo(const CurseforgeModInfo &modInfo)
{
    CurseforgeModInfoCache info;
    info.id_ = modInfo.id();
    info.name_ = modInfo.name();
    info.summary_ = modInfo.summary();
    info.slug_ = modInfo.websiteUrl().fileName();
    info.popularityScore_ = modInfo.popularityScore();
    return info;
}

QJsonObject CurseforgeModInfoCaches::CurseforgeModInfoCache::toJsonObject() const
{
    QJsonObject object;
    object.insert("id", id_);
    object.insert("name", name_);
    object.insert("summary", summary_);
    object.insert("slug", slug_);
    object.insert("popularityScore", popularityScore_);
    return object;
}

int CurseforgeModInfoCaches::CurseforgeModInfoCache::id() const
{
    return id_;
}

const QString &CurseforgeModInfoCaches::CurseforgeModInfoCache::name() const
{
    return name_;
}

const QString &CurseforgeModInfoCaches::CurseforgeModInfoCache::summary() const
{
    return summary_;
}

const QString &CurseforgeModInfoCaches::CurseforgeModInfoCache::slug() const
{
    return slug_;
}

void CurseforgeModInfoCaches::addCache(const CurseforgeModInfo &info)
{
    if(!info.id()) return;
    caches()->modCaches_.insert(info.id(), CurseforgeModInfoCache::fromInfo(info));
    caches()->writeToFile();
}

const QMap<int, CurseforgeModInfoCaches::CurseforgeModInfoCache> &CurseforgeModInfoCaches::modInfos() const
{
    return modCaches_;
}

CurseforgeModInfoCaches::CurseforgeModInfoCaches()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    dir.cd("curseforge");
    QDir().mkpath(dir.path());
    file_.setFileName(dir.absoluteFilePath(kFileName));
    readFromFile();
}

CurseforgeModInfoCaches *CurseforgeModInfoCaches::caches()
{
    static CurseforgeModInfoCaches caches;
    return &caches;
}

void CurseforgeModInfoCaches::writeToFile()
{
    QJsonObject object;
    for(auto it = modCaches_.cbegin(); it != modCaches_.cend(); it++){
        object.insert(QString::number(it.key()), it->toJsonObject());
    }
    QJsonDocument doc(object);
    if(!file_.open(QIODevice::WriteOnly)) return;
    file_.write(doc.toJson());
    file_.close();
}

void CurseforgeModInfoCaches::readFromFile()
{
    if(!file_.open(QIODevice::ReadOnly)) return;
    auto bytes = file_.readAll();
    file_.close();
    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return;
    }
    modCaches_.clear();
    auto result = jsonDocument.toVariant();
    for(auto &&variant : result.toList()){
        auto cache = CurseforgeModInfoCache::fromVariant(variant);
        modCaches_.insert(cache.id(), cache);
    }
}
