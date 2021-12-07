#ifndef MODRINTHMOD_H
#define MODRINTHMOD_H

#include <QObject>

#include "modrinthmodinfo.h"
#include "tag/tagable.h"

class ModrinthAPI;
class LocalMod;

class ModrinthMod : public QObject, public Tagable
{
    Q_OBJECT
public:
    ModrinthMod(LocalMod *parent, const QString &id);
    ModrinthMod(QObject *parent, const ModrinthModInfo &info);
    ModrinthMod(LocalMod *parent, const ModrinthModInfo &info);
    ~ModrinthMod();

    ModrinthModInfo modInfo() const;

    void acquireAuthor();
    void acquireIcon();
    void acquireFullInfo();
    QMetaObject::Connection acquireFileList(std::function<void (QList<ModrinthFileInfo>)> callback = [](QList<ModrinthFileInfo>){}, std::function<void ()> failed = []{});

    void setModInfo(ModrinthModInfo newModInfo);

signals:
    void tagsChanged() override;

    void authorReady();
    void iconReady();
    void fullInfoReady();
    void fileListReady();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();

private:
    ModrinthAPI *api_;
    ModrinthModInfo modInfo_;

    bool gettingAuthor_ = false;
    bool gettingIcon_ = false;
    bool gettingFullInfo_ = false;
    bool gettingFileList_ = false;
};

#endif // MODRINTHMOD_H
