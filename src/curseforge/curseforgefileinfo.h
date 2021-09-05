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

    const QList<GameVersion> &gameVersions() const;

    const QList<ModLoaderType::Type> &loaderTypes() const;

    int releaseType() const;

    const QDateTime &fileDate() const;

private:
    int id_;
    QList<GameVersion> gameVersions_;
    QList<ModLoaderType::Type> loaderTypes_;
    int releaseType_;
    QDateTime fileDate_;
};

#endif // CURSEFORGEMODFILEINFO_H
