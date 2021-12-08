#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>

#include "curseforgemodinfo.h"
#include "tag/tagable.h"

class LocalMod;
class CurseforgeAPI;

class CurseforgeMod : public QObject, public Tagable
{
    Q_OBJECT
public:
    CurseforgeMod(QObject *parent, int id);
    CurseforgeMod(LocalMod *parent, int id);
    CurseforgeMod(QObject *parent, const CurseforgeModInfo &info);
    CurseforgeMod(LocalMod *parent, const CurseforgeModInfo &info);
    ~CurseforgeMod();

    void acquireBasicInfo();
    void acquireIcon();
    void acquireDescription();
    QMetaObject::Connection acquireAllFileList();

    const CurseforgeModInfo &modInfo() const;
signals:
    void tagsChanged() override;

    void basicInfoReady();
    void iconReady();
    void descriptionReady();
    void allFileListReady(QList<CurseforgeFileInfo> fileInfos);

private:
    CurseforgeAPI *api_;
    CurseforgeModInfo modInfo_;

    bool gettingBasicInfo_ = false;
    bool gettingIcon_ = false;
    bool gettingDescription_ = false;
    bool gettingAllFileList_ = false;
};

#endif // CURSEFORGEMOD_H
