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

    void searchOnWebsite();
    void searchOnCurseforge();
    void searchOnModrinth();

    void checkUpdates(const GameVersion &targetVersion, ModLoaderType::Type targetType);
    void checkCurseforgeUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType);
    void checkModrinthUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType);

    ModWebsiteType updateType() const;
    void update(ModWebsiteType type);

    qint64 updateSize(ModWebsiteType type) const;

    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo() const;

    std::optional<CurseforgeFileInfo> updateCurseforgeFileInfo() const;

    std::optional<ModrinthFileInfo> currentModrinthFileInfo() const;

    std::optional<ModrinthFileInfo> updateModrinthFileInfo() const;

signals:
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
    void updateFinished();

private:
    template<typename T>
    static std::optional<T> findUpdate(QList<T> fileList, const GameVersion &targetVersion, ModLoaderType::Type targetType);

    LocalModInfo modInfo_;

    CurseforgeMod *curseforgeMod_ = nullptr;
    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo_;
    std::optional<CurseforgeFileInfo> updateCurseforgeFileInfo_;

    ModrinthMod *modrinthMod_ = nullptr;
    std::optional<ModrinthFileInfo> currentModrinthFileInfo_;
    std::optional<ModrinthFileInfo> updateModrinthFileInfo_;
};

#endif // LOCALMOD_H
