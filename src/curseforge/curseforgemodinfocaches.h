#ifndef CURSEFORGEMODINFOCACHES_H
#define CURSEFORGEMODINFOCACHES_H

#include "curseforgemodinfo.h"

class CurseforgeModInfoCaches
{
    friend class CurseforgeModInfo;
public:
    class CurseforgeModInfoCache
    {
    public:
        CurseforgeModInfoCache() = default;
        static CurseforgeModInfoCache fromVariant(const QVariant &variant);
        static CurseforgeModInfoCache fromInfo(const CurseforgeModInfo &modInfo);
        QJsonObject toJsonObject() const;

        int id() const;
        const QString &name() const;
        const QString &summary() const;
        const QString &slug() const;

    private:
        int id_;
        QString name_;
        QString summary_;
        QString slug_;
        double popularityScore_;
    };

    static void addCache(const CurseforgeModInfo &info);
    const QMap<int, CurseforgeModInfoCache> &modInfos() const;

private:
    CurseforgeModInfoCaches();
    static CurseforgeModInfoCaches *caches();
    void writeToFile();
    void readFromFile();

    static constexpr auto kFileName = "caches.json";
    QFile file_;
    QMap<int, CurseforgeModInfoCache> modCaches_;
};

#endif // CURSEFORGEMODINFOCACHES_H
