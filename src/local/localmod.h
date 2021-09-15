#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>
#include <optional>

#include "localmodfile.h"
#include "updatable.hpp"
#include "localmodfileinfo.h"
#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"

class LocalModPath;
class CurseforgeMod;
class ModrinthMod;
class CurseforgeAPI;
class ModrinthAPI;

class LocalMod : public QObject
{
    Q_OBJECT
public:
    enum ModWebsiteType{ None, Curseforge, Modrinth};

    explicit LocalMod(QObject *parent, LocalModFile *file);

    explicit LocalMod(LocalModPath *parent, LocalModFile *file);

    LocalModFileInfo modInfo() const;

    CurseforgeMod *curseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    void setModrinthMod(ModrinthMod *newModrinthMod);

    void searchOnWebsite();
    void searchOnCurseforge();
    void searchOnModrinth();

    void checkUpdates(const GameVersion &targetVersion, ModLoaderType::Type targetType);
    void checkCurseforgeUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType);
    void checkModrinthUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType);

    ModWebsiteType defaultUpdateType() const;
    QList<ModWebsiteType> updateTypes() const;
    QPair<QString, QString> updateNames(ModWebsiteType type) const;
    void update(ModWebsiteType type);

    qint64 updateSize(ModWebsiteType type) const;

    CurseforgeAPI *curseforgeAPI() const;

    ModrinthAPI *modrinthAPI() const;

    void addOldFile(LocalModFile *oldFile);

    void addDuplicateFile(LocalModFile *duplicateFile);

    void duplicateToOld();

    void rollback(LocalModFile *file);
    void deleteAllOld();

    bool rename(const QString &oldBaseName, const QString &newBaseName);

    void addDepend(std::tuple<QString, QString, std::optional<FabricModInfo>> modDepend);
    void addConflict(std::tuple<QString, QString, FabricModInfo> modConflict);
    void addBreak(std::tuple<QString, QString, FabricModInfo> modBreak);

    LocalModFile *modFile() const;
    const QList<LocalModFile *> &oldFiles() const;
    const QList<LocalModFile *> &duplicateFiles() const;
    const Updatable<CurseforgeFileInfo> &curseforgeUpdate() const;
    const Updatable<ModrinthFileInfo> &modrinthUpdate() const;
    const QList<std::tuple<QString, QString, std::optional<FabricModInfo> > > &depends() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &conflicts() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &breaks() const;

    ModrinthMod *modrinthMod() const;

signals:
    void modFileUpdated();

    void checkWebsiteStarted();
    void websiteReady(bool bl);
    void checkUpdatesStarted();
    void updateReady(ModWebsiteType type);

    void curseforgeReady(bool bl);
    void curseforgeUpdateReady(bool bl);
    void modrinthReady(bool bl);
    void modrinthUpdateReady(bool bl);

    void checkCurseforgeStarted();
    void checkCurseforgeUpdateStarted();
    void checkModrinthStarted();
    void checkModrinthUpdateStarted();

    void updateStarted();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateFinished(bool success);

private:
    //api for update
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;

    //files
    LocalModFile *modFile_;
    QList<LocalModFile*> oldFiles_;
    QList<LocalModFile*> duplicateFiles_;

    //related on websites
    CurseforgeMod *curseforgeMod_ = nullptr;
    Updatable<CurseforgeFileInfo> curseforgeUpdate_;

    ModrinthMod *modrinthMod_ = nullptr;
    Updatable<ModrinthFileInfo> modrinthUpdate_;

    //dependencies
    QList<std::tuple<QString, QString, std::optional<FabricModInfo>>> depends_;
    QList<std::tuple<QString, QString, FabricModInfo>> conflicts_;
    QList<std::tuple<QString, QString, FabricModInfo>> breaks_;
};

#endif // LOCALMOD_H
