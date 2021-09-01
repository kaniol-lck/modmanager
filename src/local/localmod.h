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
    explicit LocalMod(QObject *parent, const LocalModInfo &info);

    const LocalModInfo &getModInfo() const;

    CurseforgeMod *getCurseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    ModrinthMod *getModrinthMod() const;
    void setModrinthMod(ModrinthMod *newModrinthMod);

    void searchOnCurseforge();
    void searchOnModrinth();

    void checkCurseforgeUpdate(const GameVersion &version, ModLoaderType::Type loaderType);
    void checkModrinthUpdate(const GameVersion &version, ModLoaderType::Type loaderType);

    void update();

    std::optional<CurseforgeFileInfo> getCurrentCurseforgeFileInfo() const;

    std::optional<CurseforgeFileInfo> getUpdateCurseforgeFileInfo() const;

    std::optional<ModrinthFileInfo> getCurrentModrinthFileInfo() const;

    std::optional<ModrinthFileInfo> getUpdateModrinthFileInfo() const;

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
    LocalModInfo localModInfo;

    CurseforgeMod *curseforgeMod = nullptr;
    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo;
    std::optional<CurseforgeFileInfo> updateCurseforgeFileInfo;

    ModrinthMod *modrinthMod = nullptr;
    std::optional<ModrinthFileInfo> currentModrinthFileInfo;
    std::optional<ModrinthFileInfo> updateModrinthFileInfo;
};

#endif // LOCALMOD_H
