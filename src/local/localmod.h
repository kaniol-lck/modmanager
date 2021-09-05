#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>
#include <optional>

#include "localmodinfo.h"
#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"

class CurseforgeMod;
class ModrinthMod;

class LocalMod : public QObject
{
    Q_OBJECT
public:
    enum ModWebsiteType{ None, Curseforge, Modrinth};

    explicit LocalMod(QObject *parent, const LocalModInfo &info);

    const LocalModInfo &modInfo() const;

    CurseforgeMod *curseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    ModrinthMod *modrinthMod() const;
    void setModrinthMod(ModrinthMod *newModrinthMod);

    void searchOnCurseforge();
    void searchOnModrinth();

    void checkCurseforgeUpdate(const GameVersion &version, ModLoaderType::Type loaderType);
    void checkModrinthUpdate(const GameVersion &mainVersion, ModLoaderType::Type loaderType);

    ModWebsiteType updateType() const;
    void update(ModWebsiteType type);

    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo() const;

    std::optional<CurseforgeFileInfo> updateCurseforgeFileInfo() const;

    std::optional<ModrinthFileInfo> currentModrinthFileInfo() const;

    std::optional<ModrinthFileInfo> updateModrinthFileInfo() const;

signals:
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
    void updateFinished();

private:
    LocalModInfo modInfo_;

    CurseforgeMod *curseforgeMod_ = nullptr;
    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo_;
    std::optional<CurseforgeFileInfo> updateCurseforgeFileInfo_;

    ModrinthMod *modrinthMod_ = nullptr;
    std::optional<ModrinthFileInfo> currentModrinthFileInfo_;
    std::optional<ModrinthFileInfo> updateModrinthFileInfo_;
};

#endif // LOCALMOD_H
