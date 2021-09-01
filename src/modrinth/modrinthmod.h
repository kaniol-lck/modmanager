#ifndef MODRINTHMOD_H
#define MODRINTHMOD_H

#include <QObject>
#include <QDir>

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

    void download(const ModrinthFileInfo &fileInfo, const QDir &path = QDir());

signals:
    void iconReady();
    void fullInfoReady();
    void fileListReady();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();

private:
    ModrinthModInfo modInfo;

    bool gettingIcon = false;
    bool gettingFullInfo = false;
    bool gettingFileList = false;

};

#endif // MODRINTHMOD_H
