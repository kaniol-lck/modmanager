#ifndef CURSEFORGEMODFILEINFO_H
#define CURSEFORGEMODFILEINFO_H

#include <QVariant>
#include <QUrl>
#include <QDateTime>

#include "gameversion.h"
#include "modloadertype.h"
#include "modwebsitetype.h"
#include "curseforgedependencyinfo.h"

class CurseforgeFileInfo
{
public:
    static constexpr ModWebsiteType Type = Curseforge;
    using IdType = int;
    CurseforgeFileInfo() = default;
    explicit CurseforgeFileInfo(IdType id);

    static CurseforgeFileInfo fromVariant(const QVariant &variant);
    QJsonObject toJsonObject() const;

    IdType id() const;
    const QString &displayName() const;
    const QString &fileName() const;
    const QUrl &url() const;
    qint64 size() const;
    const QList<GameVersion> &gameVersions() const;
    const QList<ModLoaderType::Type> &loaderTypes() const;

    enum ReleaseType{
        Release = 1,
        Beta,
        Alpha
    };
    int releaseType() const;
    const QDateTime &fileDate() const;
    QList<CurseforgeDependencyInfo> &dependencies();

    bool operator==(const CurseforgeFileInfo &info) const;

private:
    IdType id_;
    QString displayName_;
    QString fileName_;
    QUrl url_;
    qint64 size_;
    QList<GameVersion> gameVersions_;
    QList<ModLoaderType::Type> loaderTypes_;
    int releaseType_;
    QDateTime fileDate_;
    QList<CurseforgeDependencyInfo> dependencies_;
};

#endif // CURSEFORGEMODFILEINFO_H
