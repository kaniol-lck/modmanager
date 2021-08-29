#ifndef MODRINTHFILEINFO_H
#define MODRINTHFILEINFO_H

#include <QVariant>
#include <QUrl>
#include <QDateTime>

#include "gameversion.h"
#include "modloadertype.h"

class ModrinthFileInfo
{
public:
    ModrinthFileInfo() = default;

    static ModrinthFileInfo fromVariant(const QVariant &variant);

    const QString &getId() const;

    const QString &getModId() const;

    const QString &getDisplayName() const;

    const QString &getFileName() const;

    const QUrl &getDownloadUrl() const;

    const QList<GameVersion> &getGameVersions() const;

    const QList<ModLoaderType::Type> &getModLoaders() const;

    int getFileLength() const;

    int getReleaseType() const;

    const QDateTime &getFileDate() const;

private:
    QString id;
    QString modId;
    QString displayName;
    QString fileName;
    QUrl downloadUrl;
    QList<GameVersion> gameVersions;
    QList<ModLoaderType::Type> modLoaders;
    int fileLength;
    int releaseType;
    QDateTime fileDate;
};

#endif // MODRINTHFILEINFO_H
