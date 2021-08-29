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
    void acquireFileList();

signals:
    void iconReady();
    void fullInfoReady();
    void fileListReady();

private:
    ModrinthModInfo modInfo;

    bool gettingIcon = false;
    bool gettingFullInfo = false;
    bool gettingFileList = false;

};

#endif // MODRINTHMOD_H
