#ifndef CURSEFORGEMODFILEINFO_H
#define CURSEFORGEMODFILEINFO_H

#include <QVariant>
#include <QUrl>

#include "gameversion.h"
#include "modloadertype.h"

class CurseforgeFileInfo
{
public:
    CurseforgeFileInfo();

    static CurseforgeFileInfo fromVariant(const QVariant &variant);

    int getId() const;

    const QString &getDisplayName() const;

    const QUrl &getDownloadUrl() const;

    const QList<GameVersion> &getGameVersions() const;

    const QList<ModLoaderType::Type> &getModLoaders() const;

    int getFileLength() const;

    const QString &getFileName() const;

    int getReleaseType() const;

private:
    int id;
    QString displayName;
    QString fileName;
    QUrl downloadUrl;
    QList<GameVersion> gameVersions;
    QList<ModLoaderType::Type> modLoaders;
    int fileLength;
    int releaseType;
};

#endif // CURSEFORGEMODFILEINFO_H
