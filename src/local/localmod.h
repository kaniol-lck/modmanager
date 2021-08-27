#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>
#include <optional>

#include "localmodinfo.h"
#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;

class LocalMod : public QObject
{
    Q_OBJECT
public:
    explicit LocalMod(QObject *parent, const LocalModInfo &info);

    const LocalModInfo &getModInfo() const;

    CurseforgeMod *getCurseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    void searchOnCurseforge();

    void checkUpdate(const GameVersion &version, ModLoaderType::Type loaderType);

    void update(bool deleteOld = true);

    std::optional<CurseforgeFileInfo> getCurrentCurseforgeFileInfo() const;

    std::optional<CurseforgeFileInfo> getUpdateFileInfo() const;

signals:
    void curseforgeReady(bool bl);
    void updateReady(bool bl);

    void checkCurseforgeStarted();
    void checkUpdateStarted();
    void updateStarted();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateFinished();

private:
    LocalModInfo localModInfo;
    CurseforgeMod *curseforgeMod = nullptr;
    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo;
    std::optional<CurseforgeFileInfo> updateFileInfo;

};

#endif // LOCALMOD_H
