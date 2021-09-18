#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>

#include "curseforgemodinfo.h"

class LocalMod;
class CurseforgeAPI;

class CurseforgeMod : public QObject
{
    Q_OBJECT
public:
    CurseforgeMod(LocalMod *parent, int id);
    CurseforgeMod(QObject *parent, const CurseforgeModInfo &info);
    CurseforgeMod(LocalMod *parent, const CurseforgeModInfo &info);

    void acquireBasicInfo();
    void acquireIcon();
    void acquireDescription();
    void acquireAllFileList();

    const CurseforgeModInfo &modInfo() const;

signals:
    void basicInfoReady();
    void iconReady();
    void descriptionReady();
    void allFileListReady();

private:
    CurseforgeAPI *api_;
    CurseforgeModInfo modInfo_;

    bool gettingBasicInfo_ = false;
    bool gettingIcon_ = false;
    bool gettingDescription_ = false;
    bool gettingAllFileList_ = false;
};

#endif // CURSEFORGEMOD_H
