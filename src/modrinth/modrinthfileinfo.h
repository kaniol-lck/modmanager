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
    using IdType = QString;
    ModrinthFileInfo() = default;

    static ModrinthFileInfo fromVariant(const QVariant &variant);

    const IdType &id() const;
    const QString &modId() const;
    const QString &displayName() const;
    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;
    const QList<GameVersion> &gameVersions() const;
    const QList<ModLoaderType::Type> &loaderTypes() const;
    int releaseType() const;
    const QDateTime &fileDate() const;
    bool isFeatured() const;
    bool isPrimary() const;

private:
    IdType id_;
    QString modId_;
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_;
    QList<GameVersion> gameVersions_;
    QList<ModLoaderType::Type> loaderTypes_;
    int releaseType_;
    QDateTime fileDate_;
    bool isFeatured_;
    bool isPrimary_;
};

#endif // MODRINTHFILEINFO_H
