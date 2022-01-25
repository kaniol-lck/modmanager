#ifndef LOCALFILELINKER_H
#define LOCALFILELINKER_H

#include <QObject>
#include <optional>

#include "network/reply.hpp"
#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"
#include "util/checksheet.h"

class LocalModFile;
class LocalFileLinker : public QObject
{
    Q_OBJECT
public:
    explicit LocalFileLinker(LocalModFile *localFile);

    bool linked() const;

    void link();
    void linkCached();
    void linkCachedCurseforge();
    void linkCurseforge();
    void linkCachedModrinth();
    void linkModrinth();
    std::optional<CurseforgeFileInfo> curseforgeFileInfo() const;
    void setCurseforgeFileInfo(CurseforgeFileInfo newCurseforgeFileInfo);

    std::optional<ModrinthFileInfo> modrinthFileInfo() const;
    void setModrinthFileInfo(ModrinthFileInfo newModrinthFileInfo);

signals:
    void linkStarted();
    void linkFinished(bool success);
    void linkCurseforgeStarted();
    void linkCurseforgeFinished(bool success, int id = 0);
    void linkModrinthStarted();
    void linkModrinthFinished(bool success, QString id = QString());
    void curseforgeFileInfoChanged();
    void modrinthFileInfoChanged();

private:
    LocalModFile *localFile_ = nullptr;
    std::optional<CurseforgeFileInfo> curseforgeFileInfo_;
    std::optional<ModrinthFileInfo> modrinthFileInfo_;

    bool curseforgeLinked_ = false;
    bool modrinthLinked_ = false;
    std::unique_ptr<Reply<int, CurseforgeFileInfo, QList<CurseforgeFileInfo>>> curseforgeSearcher_;
    std::unique_ptr<Reply<ModrinthFileInfo>> modrinthSearcher_;
};

#endif // LOCALFILELINKER_H
