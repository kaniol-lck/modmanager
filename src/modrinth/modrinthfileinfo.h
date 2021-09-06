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

    const QString &id() const;

    const QString &modId() const;

    const QList<GameVersion> &gameVersions() const;

    const QList<ModLoaderType::Type> &loaderTypes() const;

    int size() const;

    int releaseType() const;

    const QDateTime &fileDate() const;

    bool isFeatured() const;

private:
    QString id_;
    QString modId_;
    QList<GameVersion> gameVersions_;
    QList<ModLoaderType::Type> loaderTypes_;
    int size_;
    int releaseType_;
    QDateTime fileDate_;
    bool isFeatured_;
};

#endif // MODRINTHFILEINFO_H
