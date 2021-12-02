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

    void link();
    void linkCurseforge();
    void linkModrinth();
    std::optional<CurseforgeFileInfo> curseforgeFile() const;
    void setCurseforgeFile(CurseforgeFileInfo newCurseforgeFile);

    std::optional<ModrinthFileInfo> modrinthFile() const;
    void setModrinthFile(ModrinthFileInfo newModrinthFile);

signals:
    void linkStarted();
    void linkFinished(bool success);
    void linkCurseforgeStarted();
    void linkCurseforgeFinished(bool success);
    void linkModrinthStarted();
    void linkModrinthFinished(bool success);
    void curseforgeFileChanged(std::optional<CurseforgeFileInfo> curseforgeFile);
    void modrinthFileChanged(std::optional<ModrinthFileInfo> modrinthFile);

private:
    LocalModFile *localFile_ = nullptr;
    std::optional<CurseforgeFileInfo> curseforgeFile_;
    std::optional<ModrinthFileInfo> modrinthFile_;
};

#endif // LOCALFILELINKER_H
