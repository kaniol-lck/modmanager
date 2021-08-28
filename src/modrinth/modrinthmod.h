#ifndef MODRINTHMOD_H
#define MODRINTHMOD_H

#include <QObject>

#include "modrinthmodinfo.h"

class ModrinthMod : public QObject
{
    Q_OBJECT
public:
    ModrinthMod(QObject *parent, const ModrinthModInfo &info);

    ModrinthModInfo getModInfo() const;

    void acquireIcon();

signals:
    void iconReady();

private:
    ModrinthModInfo modInfo;

    bool gettingIcon = false;

};

#endif // MODRINTHMOD_H
