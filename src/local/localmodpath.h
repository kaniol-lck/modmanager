#ifndef LOCALMODPATH_H
#define LOCALMODPATH_H

#include <QFileSystemWatcher>
#include <QObject>

#include "localmod.h"
#include "localmodpathinfo.h"
#include "util/checksheet.h"

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

    QString modsJsonFilePath() const;

    void loadMods(bool autoLoaderType = false);
    void addModFile(LocalModFile *file);
    void removeModFile(LocalModFile *file);

    LocalModPath *addSubPath(const QString &relative);

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
    QAria2Downloader *downloadNewMod(CurseforgeMod *mod, CurseforgeFile *file);

    const LocalModPathInfo &info() const;
    void setInfo(const LocalModPathInfo &newInfo, bool deduceLoader = false);

    Tagable &containedTags();

    CurseforgeAPI *curseforgeAPI() const;
    ModrinthAPI *modrinthAPI() const;
    int updatableCount() const;
    const QMap<QString, LocalMod *> &modMap() const;
    QList<LocalMod *> modList() const;
    void deleteAllOld() const;

    LocalMod *optiFineMod() const;

    bool isLoading() const;
    bool isLinking() const;
    bool isChecking() const;
    bool modsLoaded() const;
    bool isUpdating() const;

    const CheckSheet *updateChecker() const;
    const CheckSheet *modsLinker() const;

    QString displayName() const;
    QIcon icon() const;

    const QMap<QString, LocalModPath *> &subPaths() const;

    const QStringList &relative() const;

    const QStringList &nonModFiles() const;

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

    //update
    void updatesStarted();
    void updatableCountChanged(int count);
    void updatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updatesDoneCountUpdated(int doneCount, int totalCount);
    void updatesDone(int successCount, int failCount);
private slots:
    void updateUpdatableCount();
    void onDirectoryChanged(const QString &file);

private:
    enum FindResultType{ Environmant, Missing, Mismatch, Match, RangeSemverError, VersionSemverError };
    std::tuple<FindResultType, std::optional<FabricModInfo>> findFabricMod(const QString &modid, const QString &range_str) const;

    explicit LocalModPath(LocalModPath *path, const QString &subDir);

    QFileSystemWatcher watcher_;
    QStringList nonModFiles_;
    QStringList relative_;
    QMap<QString, LocalModPath *> subPaths_;
    Tagable containedTags_;
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;
    LocalModPathInfo info_;
    QMap<QString, LocalMod*> modMap_;
    LocalMod *optiFineMod_ = nullptr;
    QMultiMap<QString, FabricModInfo> fabricModMap_;
    QStringList provideList_;
    QDateTime latestUpdateCheck_;
    CheckSheet updateChecker_;
    CheckSheet modsLinker_;
    int updatableCount_ = 0;
    bool loaded_ = false;
    bool initialUpdateChecked_ = false;
    bool isLoading_ = false;
    bool isUpdating_ = false;
};

#endif // LOCALMODPATH_H
