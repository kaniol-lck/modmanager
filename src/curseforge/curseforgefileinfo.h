#ifndef CURSEFORGEMODFILEINFO_H
#define CURSEFORGEMODFILEINFO_H

#include <QVariant>
#include <QUrl>

#include "gameversion.h"

class CurseforgeFileInfo
{
public:
    CurseforgeFileInfo();

    static CurseforgeFileInfo fromVariant(const QVariant &variant);

    int getId() const;

    const QString &getDisplayName() const;

    const QUrl &getDownloadUrl() const;

    const QList<GameVersion> &getGameVersions() const;

    const QStringList &getModLoaders() const;

    int getFileLength() const;

    const QString &getFileName() const;

private:
    int id;
    QString displayName;
    QString fileName;
    QUrl downloadUrl;
    QList<GameVersion> gameVersions;
    QStringList modLoaders;
    int fileLength;
};

#endif // CURSEFORGEMODFILEINFO_H
