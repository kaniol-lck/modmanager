#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>
#include <optional>

#include "localmodfile.h"
#include "updatable.hpp"
#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"
#include "tag/localmodtags.h"
#include "modwebsitetype.h"

class LocalModPath;
class CurseforgeMod;
class ModrinthMod;
class CurseforgeAPI;
class ModrinthAPI;

class LocalMod : public QObject
{
    Q_OBJECT
public:
    explicit LocalMod(QObject *parent, LocalModFile *file);
    explicit LocalMod(LocalModPath *parent, LocalModFile *file);
    ~LocalMod();

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
    void checkUpdates(bool force = true);
    void checkCurseforgeUpdate(bool force = true);
    void checkModrinthUpdate(bool force = true);

    QAria2Downloader *downloadOldMod(DownloadFileInfo &info);

    ModWebsiteType defaultUpdateType() const;
    QList<ModWebsiteType> updateTypes() const;
    QPair<QString, QString> updateNames(ModWebsiteType type) const;
    QPair<QString, QString> updateInfos(ModWebsiteType type) const;
    QAria2Downloader *update(ModWebsiteType type);

    qint64 updateSize(ModWebsiteType type) const;

    //api
    CurseforgeAPI *curseforgeAPI() const;
    ModrinthAPI *modrinthAPI() const;

    //old
    void addOldFile(LocalModFile *oldFile);
    const QList<LocalModFile *> &oldFiles() const;

    //duplicate
    void addDuplicateFile(LocalModFile *duplicateFile);
    const QList<LocalModFile *> &duplicateFiles() const;

    void duplicateToOld();
    void rollback(LocalModFile *file);
    void deleteAllOld();
    void deleteOld(LocalModFile *file);

    bool isDisabled();
    bool setEnabled(bool enabled);

    void ignoreUpdate(ModWebsiteType type);
    void clearIgnores();

    void addDepend(std::tuple<QString, QString, std::optional<FabricModInfo>> modDepend);
    void addConflict(std::tuple<QString, QString, FabricModInfo> modConflict);
    void addBreak(std::tuple<QString, QString, FabricModInfo> modBreak);

    const Updatable<CurseforgeFileInfo> &curseforgeUpdate() const;
    const Updatable<ModrinthFileInfo> &modrinthUpdate() const;
    void setCurrentCurseforgeFileInfo(const CurseforgeFileInfo &info, bool cache = true);
    void setCurrentModrinthFileInfo(const ModrinthFileInfo &info, bool cache = true);
    const QList<std::tuple<QString, QString, std::optional<FabricModInfo> > > &depends() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &conflicts() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &breaks() const;

    void setCurseforgeId(int id, bool cache = true);
    void setModrinthId(const QString &id, bool cache = true);

    const QString &alias() const;
    void setAlias(const QString &newAlias);

    QJsonObject toJsonObject() const;
    void restore(const QVariant &variant);

    bool isFeatured() const;
    void setFeatured(bool featured);

    LocalModPath *path() const;

    const QList<Tag> tags() const;
    const QList<Tag> customizableTags() const;
    void addTag(const Tag &tag);
    void removeTag(const Tag &tag);
    const LocalModTags &tagManager() const;

    void setModFile(LocalModFile *newModFile);

    const QPixmap &icon() const;

signals:
    void modFileUpdated();
    void modIconUpdated();
    void modCacheUpdated();

    void checkWebsiteStarted();
    void websiteReady(bool bl);
    void checkUpdatesStarted();
    void updateReady(QList<ModWebsiteType> types, bool success = true);
    void searchOnCurseforgeFinished(bool bl);
    void searchOnModrinthFinished(bool bl);

    void curseforgeReady(bool bl);
    void curseforgeUpdateReady(bool hasUpdate, bool success = true);
    void modrinthReady(bool bl);
    void modrinthUpdateReady(bool hasUpdate, bool success = true);

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
    LocalModTags tagManager_;
    QPixmap icon_;

    //api for update
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;

    //files
    LocalModPath *path_;
    LocalModFile *modFile_;
    QList<LocalModFile*> oldFiles_;
    QList<LocalModFile*> duplicateFiles_;

    //target for update
    GameVersion targetVersion_;
    ModLoaderType::Type targetLoaderType_;

    //related on websites
    CurseforgeMod *curseforgeMod_ = nullptr;
    Updatable<CurseforgeFileInfo> curseforgeUpdate_;

    ModrinthMod *modrinthMod_ = nullptr;
    Updatable<ModrinthFileInfo> modrinthUpdate_;

    //dependencies
    QList<std::tuple<QString, QString, std::optional<FabricModInfo>>> depends_;
    QList<std::tuple<QString, QString, FabricModInfo>> conflicts_;
    QList<std::tuple<QString, QString, FabricModInfo>> breaks_;

    void updateIcon();
};

#endif // LOCALMOD_H
