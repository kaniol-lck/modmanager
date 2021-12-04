#ifndef MODRINTHFILEINFO_H
#define MODRINTHFILEINFO_H

#include <QVariant>
#include <QUrl>
#include <QDateTime>

#include "gameversion.h"
#include "modloadertype.h"
#include "modwebsitetype.h"

class ModrinthFileInfo
{
public:
    static constexpr ModWebsiteType Type = Modrinth;
    using IdType = QString;
    ModrinthFileInfo() = default;
    explicit ModrinthFileInfo(IdType id);

    static ModrinthFileInfo fromVariant(const QVariant &variant);
    QJsonObject toJsonObject() const;

    const IdType &id() const;
    const QString &modId() const;
    const QString &displayName() const;
    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;
    const QList<GameVersion> &gameVersions() const;
    const QList<ModLoaderType::Type> &loaderTypes() const;
    static constexpr auto Release = "release";
    static constexpr auto Beta = "beta";
    static constexpr auto Alpha = "alpha";
    const QString &releaseType() const;
    const QDateTime &fileDate() const;
    bool isFeatured() const;
    bool isPrimary() const;

    bool operator==(const ModrinthFileInfo &info) const;

private:
    IdType id_;
    QString modId_;
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_;
    QList<GameVersion> gameVersions_;
    QList<ModLoaderType::Type> loaderTypes_;
    QString releaseType_;
    QDateTime fileDate_;
    bool isFeatured_;
    bool isPrimary_;
};

#endif // MODRINTHFILEINFO_H
