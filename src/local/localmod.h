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

    void findUpdate(const GameVersion &version, ModLoaderType::Type loaderType);

    std::optional<CurseforgeFileInfo> getCurrentCurseforgeFileInfo() const;

    std::optional<CurseforgeFileInfo> getUpdateFileInfo() const;

signals:
    void curseforgeReady();
    void needUpdate();

private:
    LocalModInfo localModInfo;
    CurseforgeMod *curseforgeMod = nullptr;
    std::optional<CurseforgeFileInfo> currentCurseforgeFileInfo;
    std::optional<CurseforgeFileInfo> updateFileInfo;

};

#endif // LOCALMOD_H
