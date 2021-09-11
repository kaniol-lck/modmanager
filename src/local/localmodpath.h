#ifndef LOCALMODPATH_H
#define LOCALMODPATH_H

#include <QObject>

#include "localmod.h"
#include "localmodpathinfo.h"

class LocalMod;
class CurseforgeAPI;
class ModrinthAPI;

class LocalModPath : public QObject
{
    Q_OBJECT
public:
    explicit LocalModPath(QObject *parent, const LocalModPathInfo &info);

    void reload();

    void searchOnWebsites();
    void checkModUpdates();
    void updateMods(QList<QPair<LocalMod *, LocalMod::ModWebsiteType> > modUpdateList);

    const LocalModPathInfo &info() const;
    void setInfo(const LocalModPathInfo &newInfo);

    const QList<LocalMod*> &modList() const;

    CurseforgeAPI *curseforgeAPI() const;

    ModrinthAPI *modrinthAPI() const;

    int updatableCount() const;

signals:
    void modListUpdated();

    void checkWebsitesStarted();
    void websiteCheckedCountUpdated(int checkedCount);
    void websitesReady();

    void checkUpdatesStarted();
    void updateCheckedCountUpdated(int updateCount, int checkedCount);
    void updatesReady();

    void updatesStarted();
    void updatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updatesDoneCountUpdated(int doneCount, int totalCount);
    void updatesDone(int successCount, int failCount);

private:
    LocalModPathInfo info_;
    QList<LocalMod*> modList_;
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;
    int updatableCount_;
};

#endif // LOCALMODPATH_H
