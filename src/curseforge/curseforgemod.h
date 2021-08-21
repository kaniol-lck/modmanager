#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>
#include <QVariant>
#include <QUrl>

#include "curseforgemodinfo.h"

class QNetworkAccessManager;

class CurseforgeMod : public QObject
{
    Q_OBJECT
public:
    CurseforgeMod(QObject *parent, const CurseforgeModInfo &info);

    void acquireBasicInfo();
    void acquireThumbnail();
    void acquireDescription();
    void acquireAllFileList();

    const CurseforgeModInfo &getModInfo() const;

signals:
    void basicInfoReady();
    void thumbnailReady();
    void descriptionReady();
    void allFileListReady();

private:
    CurseforgeModInfo curseforgeModInfo;

    bool gettingBasicInfo = false;
    bool gettingThumbnail = false;
    bool gettingDescription = false;
    bool gettingAllFileList = false;

};

#endif // CURSEFORGEMOD_H
