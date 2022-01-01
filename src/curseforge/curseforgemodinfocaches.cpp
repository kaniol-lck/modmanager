#include "curseforgemodinfocaches.h"

#include "util/tutil.hpp"

void CurseforgeModInfoCaches::addCache(const CurseforgeModCacheInfo &info)
{
    if(!info.id()) return;
    if(caches()->modCaches_.contains(info.id()) && caches()->modCaches_.value(info.id()) == info) return;
    caches()->modCaches_.insert(info.id(), info);
    caches()->writeToFile();
}

const QMap<int, CurseforgeModCacheInfo> &CurseforgeModInfoCaches::modInfos() const
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
    for(auto &&variant : result.toMap()){
        auto cache = CurseforgeModCacheInfo::fromVariant(variant);
        modCaches_.insert(cache.id(), cache);
    }
}
