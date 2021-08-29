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
    void acquireFullInfo();

signals:
    void iconReady();
    void fullInfoReady();

private:
    ModrinthModInfo modInfo;

    bool gettingIcon = false;
    bool gettingFullInfo = false;

};

#endif // MODRINTHMOD_H
