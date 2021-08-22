#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>

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

signals:
    void curseforgeReady();
    void needUpdate(CurseforgeFileInfo fileInfo);

private:
    LocalModInfo localModInfo;
    CurseforgeMod *curseforgeMod = nullptr;
    CurseforgeFileInfo currentCurseforgeFileInfo;

};

#endif // LOCALMOD_H
