#ifndef MODRINTHMOD_H
#define MODRINTHMOD_H

#include <QObject>

#include "modrinthmodinfo.h"

class ModrinthMod : public QObject
{
    Q_OBJECT
public:
    ModrinthMod(QObject *parent, const ModrinthModInfo &info);

    ModrinthModInfo modInfo() const;

    void acquireIcon();
    void acquireFullInfo();
    void acquireFileList();

signals:
    void iconReady();
    void fullInfoReady();
    void fileListReady();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();

private:
    ModrinthModInfo modInfo_;

    bool gettingIcon_ = false;
    bool gettingFullInfo_ = false;
    bool gettingFileList_ = false;

};

#endif // MODRINTHMOD_H
