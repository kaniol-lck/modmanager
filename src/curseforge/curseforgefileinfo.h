#ifndef CURSEFORGEMODFILEINFO_H
#define CURSEFORGEMODFILEINFO_H

#include <QVariant>
#include <QUrl>
#include <QDateTime>

#include "download/downloadfileinfo.h"
#include "gameversion.h"
#include "modloadertype.h"

class CurseforgeFileInfo : public DownloadFileInfo
{
public:
    CurseforgeFileInfo() = default;

    static CurseforgeFileInfo fromVariant(const QVariant &variant);

    SourceType source() const override;

    int getId() const;

    const QList<GameVersion> &getGameVersions() const;

    const QList<ModLoaderType::Type> &getModLoaders() const;

    int getReleaseType() const;

    const QDateTime &getFileDate() const;

private:
    int id;
    QList<GameVersion> gameVersions;
    QList<ModLoaderType::Type> modLoaders;
    int releaseType;
    QDateTime fileDate;
};

#endif // CURSEFORGEMODFILEINFO_H
