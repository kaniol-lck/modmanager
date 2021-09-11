#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>
#include <optional>

#include "updatable.hpp"
#include "localmodinfo.h"
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

    explicit LocalMod(QObject *parent, const LocalModInfo &info);

    explicit LocalMod(LocalModPath *parent, const LocalModInfo &info);

    const LocalModInfo &modInfo() const;
    void setModInfo(const LocalModInfo &newModInfo);

    CurseforgeMod *curseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    ModrinthMod *modrinthMod() const;
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

signals:
    void modInfoUpdated();

    void checkWebsiteStarted();
    void websiteReady(bool bl);
    void checkUpdatesStarted();
    void updateReady(bool bl);

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
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;
    LocalModInfo modInfo_;

    CurseforgeMod *curseforgeMod_ = nullptr;
    Updatable<CurseforgeFileInfo> curseforgeUpdate_;

    ModrinthMod *modrinthMod_ = nullptr;
    Updatable<ModrinthFileInfo> modrinthUpdate_;
};

#endif // LOCALMOD_H
