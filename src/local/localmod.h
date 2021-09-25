#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>
#include <optional>

#include "localmodfile.h"
#include "updatable.hpp"
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

    //info
    LocalModFile *modFile() const;
    const CommonModInfo *commonInfo() const;
    QString displayName() const;

    //link
    CurseforgeMod *curseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    ModrinthMod *modrinthMod() const;
    void setModrinthMod(ModrinthMod *newModrinthMod);

    //search
    void searchOnWebsite();
    void searchOnCurseforge();
    void searchOnModrinth();

    //update
    bool perpareUpdate();
    void checkUpdates(const GameVersion &targetVersion, ModLoaderType::Type targetType);
    void checkCurseforgeUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType);
    void checkModrinthUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType);

    ModWebsiteType defaultUpdateType() const;
    QList<ModWebsiteType> updateTypes() const;
    QPair<QString, QString> updateNames(ModWebsiteType type) const;
    ModDownloader *update(ModWebsiteType type);

    qint64 updateSize(ModWebsiteType type) const;

    //api
    CurseforgeAPI *curseforgeAPI() const;
    ModrinthAPI *modrinthAPI() const;

    //old
    void addOldFile(LocalModFile *oldFile);
    const QList<LocalModFile *> &oldFiles() const;

    //duplicateP
    void addDuplicateFile(LocalModFile *duplicateFile);
    const QList<LocalModFile *> &duplicateFiles() const;

    void duplicateToOld();
    void rollback(LocalModFile *file);
    void deleteAllOld();
    void deleteOld(LocalModFile *file);

    bool isDisabled();
    bool setEnabled(bool enabled);

    void addDepend(std::tuple<QString, QString, std::optional<FabricModInfo>> modDepend);
    void addConflict(std::tuple<QString, QString, FabricModInfo> modConflict);
    void addBreak(std::tuple<QString, QString, FabricModInfo> modBreak);

    const Updatable<CurseforgeFileInfo> &curseforgeUpdate() const;
    const Updatable<ModrinthFileInfo> &modrinthUpdate() const;
    const QList<std::tuple<QString, QString, std::optional<FabricModInfo> > > &depends() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &conflicts() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &breaks() const;

    void setCurseforgeId(int id);
    void setModrinthId(const QString &id);

    const QString &alias() const;
    void setAlias(const QString &newAlias);

    QJsonObject toJsonObject() const;
    void restore(const QVariant &variant);

    bool isFeatured() const;
    void setFeatured(bool featured);

    LocalModPath *path() const;

signals:
    void modFileUpdated();
    void modCacheUpdated();

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
    bool isFeatured_ = false;
    QString alias_;

    //api for update
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;

    //files
    LocalModPath *path_;
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
