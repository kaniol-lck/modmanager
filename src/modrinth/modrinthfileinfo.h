#ifndef MODRINTHFILEINFO_H
#define MODRINTHFILEINFO_H

#include <QVariant>
#include <QUrl>
#include <QDateTime>

#include "download/downloadfileinfo.h"
#include "gameversion.h"
#include "modloadertype.h"

class ModrinthFileInfo : public DownloadFileInfo
{
public:
    ModrinthFileInfo() = default;

    static ModrinthFileInfo fromVariant(const QVariant &variant);

    SourceType source() const override;

    const QString &getId() const;

    const QString &getModId() const;

    const QList<GameVersion> &getGameVersions() const;

    const QList<ModLoaderType::Type> &getModLoaders() const;

    int getFileLength() const;

    int getReleaseType() const;

    const QDateTime &getFileDate() const;

private:
    QString id;
    QString modId;
    QList<GameVersion> gameVersions;
    QList<ModLoaderType::Type> modLoaders;
    int fileLength;
    int releaseType;
    QDateTime fileDate;
};

#endif // MODRINTHFILEINFO_H
