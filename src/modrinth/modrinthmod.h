#ifndef MODRINTHMOD_H
#define MODRINTHMOD_H

#include <QObject>

#include "network/reply.hpp"
#include "modrinthmodinfo.h"
#include "tag/tagable.h"

class LocalModPath;
class ModrinthAPI;
class LocalMod;
class QAria2Downloader;
class ModrinthMod : public QObject, public Tagable
{
    Q_OBJECT
public:
    ModrinthMod(LocalMod *parent, const QString &id);
    ModrinthMod(QObject *parent, const ModrinthModInfo &info);
    ModrinthMod(LocalMod *parent, const ModrinthModInfo &info);
    ~ModrinthMod();

    ModrinthModInfo modInfo() const;

//    void acquireAuthor();
    void acquireIcon();
    std::shared_ptr<Reply<ModrinthModInfo>> acquireFullInfo();
    std::shared_ptr<Reply<QList<ModrinthFileInfo>>> acquireFileList();

    void download(const ModrinthFileInfo &fileInfo, LocalModPath *downloadPath = nullptr);

    void setModInfo(ModrinthModInfo newModInfo);

    QAria2Downloader *downloader() const;

signals:
    void tagsChanged() override;

    void authorReady();
    void iconReady();
    void fullInfoReady();
    void fileListReady(QList<ModrinthFileInfo> fileList);

    void downloadStarted();

private:
    ModrinthAPI *api_;
    ModrinthModInfo modInfo_;
    QAria2Downloader *downloader_;

//    bool gettingAuthor_ = false;
    bool gettingIcon_ = false;
    std::shared_ptr<Reply<ModrinthModInfo>> fullInfoGetter_;
    std::shared_ptr<Reply<QList<ModrinthFileInfo>>> fileListGetter_;
};

#endif // MODRINTHMOD_H
