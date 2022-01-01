#ifndef CURSEFORGEMODINFOCACHES_H
#define CURSEFORGEMODINFOCACHES_H

#include "curseforgemodinfo.h"
#include "curseforgemodcacheinfo.h"

#include <QFile>

class CurseforgeModInfoCaches
{
    friend class CurseforgeModInfo;
public:
    static void addCache(const CurseforgeModCacheInfo &info);
    const QMap<int, CurseforgeModCacheInfo> &modInfos() const;

private:
    CurseforgeModInfoCaches();
    static CurseforgeModInfoCaches *caches();
    void writeToFile();
    void readFromFile();

    static constexpr auto kFileName = "caches.json";
    QFile file_;
    QMap<int, CurseforgeModCacheInfo> modCaches_;
};

#endif // CURSEFORGEMODINFOCACHES_H
