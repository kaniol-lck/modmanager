#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>
#include <QDir>

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

    void download(const CurseforgeFileInfo &fileInfo, const QDir &path = QDir());

    const CurseforgeModInfo &getModInfo() const;

signals:
    void basicInfoReady();
    void thumbnailReady();
    void descriptionReady();
    void allFileListReady();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();

private:
    CurseforgeModInfo curseforgeModInfo;

    bool gettingBasicInfo = false;
    bool gettingThumbnail = false;
    bool gettingDescription = false;
    bool gettingAllFileList = false;

};

#endif // CURSEFORGEMOD_H
