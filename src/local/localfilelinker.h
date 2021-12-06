#ifndef LOCALFILELINKER_H
#define LOCALFILELINKER_H

#include <QObject>
#include <optional>

#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"

class LocalModFile;
class LocalFileLinker : public QObject
{
    Q_OBJECT
public:
    explicit LocalFileLinker(LocalModFile *localFile);

    bool linked() const;

    void link();
    void linkCurseforge();
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
};

#endif // LOCALFILELINKER_H
