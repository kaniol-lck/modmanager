#ifndef LOCALMODPATH_H
#define LOCALMODPATH_H

#include <QObject>

#include "localmod.h"
#include "localmodpathinfo.h"

class LocalModFile;
class LocalMod;
class CurseforgeAPI;
class ModrinthAPI;

class LocalModPath : public QObject, public Tagable
{
    Q_OBJECT
    static constexpr auto kFileName = "mods.json";
public:
    explicit LocalModPath(const LocalModPathInfo &info);
    ~LocalModPath();

    void loadMods(bool autoLoaderType = false);
    void addNormalMod(LocalModFile *file);
    void addOldMod(LocalModFile *file);

    void checkFabric();

    //list of <mod, modid, required version, current mod if present>
    QList<std::tuple<FabricModInfo, QString, QString, std::optional<FabricModInfo>>> checkFabricDepends() const;
    //list of <mod, conflict modid, conflict version, current mod>
    QList<std::tuple<FabricModInfo, QString, QString, FabricModInfo>> checkFabricConflicts() const;
    //list of <mod, break modid, break version, current mod>
    QList<std::tuple<FabricModInfo, QString, QString, FabricModInfo>> checkFabricBreaks() const;

    LocalMod *findLocalMod(const QString &id);

    //link mods
    void linkAllFiles();

    //check update
    void checkModUpdates();
    void cancelChecking();
    void updateMods(QList<QPair<LocalMod *, CurseforgeFileInfo> > curseforgeUpdateList, QList<QPair<LocalMod *, ModrinthFileInfo> > modrinthUpdateList);

    //download new mod
    QAria2Downloader *downloadNewMod(DownloadFileInfo &info);

    const LocalModPathInfo &info() const;
    void setInfo(const LocalModPathInfo &newInfo, bool deduceLoader = false);

    Tagable containedTags();

    CurseforgeAPI *curseforgeAPI() const;
    ModrinthAPI *modrinthAPI() const;
    int modCount() const;
    int updatableCount() const;
    const QMap<QString, LocalMod *> &modMap() const;
    QList<QMap<QString, LocalMod *>> modMaps() const;
    void deleteAllOld() const;

    LocalMod *optiFineMod() const;

    bool isLoading() const;
    bool isLinking() const;
    bool isChecking() const;
    bool modsLoaded() const;
    bool isUpdating() const;

public slots:
    void writeToFile();
    void readFromFile();

signals:
    void tagsChanged() override;

    void infoUpdated();
    void modListUpdated();

    //load
    void loadStarted();
    void loadProgress(int loadedCount, int totalCount);
    void loadFinished();
    void checkCancelled();

    //link
    void linkStarted();
    void linkProgress(int linkedCount, int totalCount);
    void linkFinished();

    //check update
    void checkUpdatesStarted();
    void updateCheckedCountUpdated(int updateCount, int checkedCount, int totalCount);
    void updatesReady(int failedCount = 0);

    //update
    void updatesStarted();
    void updatableCountChanged(int count);
    void updatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updatesDoneCountUpdated(int doneCount, int totalCount);
    void updatesDone(int successCount, int failCount);
private slots:
    void updateUpdatableCount();
private:
    enum FindResultType{ Environmant, Missing, Mismatch, Match, RangeSemverError, VersionSemverError };
    std::tuple<FindResultType, std::optional<FabricModInfo>> findFabricMod(const QString &modid, const QString &range_str) const;

    explicit LocalModPath(LocalModPath *path, const QString &subDir);

    QStringList relative_;
    QMap<QString, LocalModPath *> subPaths_;
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;
    LocalModPathInfo info_;
    QMap<QString, LocalMod*> modMap_;
    LocalMod *optiFineMod_ = nullptr;
    QMultiMap<QString, FabricModInfo> fabricModMap_;
    QStringList provideList_;
    QDateTime latestUpdateCheck_;
    int updatableCount_ = 0;
    bool loaded_ = false;
    bool initialUpdateChecked_ = false;
    bool isLoading_ = false;
    bool isLinking_ = false;
    bool isChecking_ = false;
    bool isUpdating_ = false;
};

#endif // LOCALMODPATH_H
