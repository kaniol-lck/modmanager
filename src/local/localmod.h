#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QObject>

#include "localmodinfo.h"

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

signals:
    void curseforgeReady();

private:
    LocalModInfo localModInfo;
    CurseforgeMod *curseforgeMod = nullptr;

};

#endif // LOCALMOD_H
