#ifndef MODRINTHMODINFO_H
#define MODRINTHMODINFO_H

#include <QVariant>
#include <QUrl>

#include "modrinthfileinfo.h"

class ModrinthModInfo
{
    friend class ModrinthMod;
public:
    ModrinthModInfo() = default;

    static ModrinthModInfo fromSearchVariant(const QVariant &variant);
    static ModrinthModInfo fromVariant(const QVariant &variant);

    const QString &getId() const;

    const QString &getAuthor() const;

    const QString &getName() const;

    const QString &getDescription() const;

    const QStringList &getVersions() const;

    int getDownloadCount() const;

    const QUrl &getIconUrl() const;

    const QByteArray &getIconBytes() const;

    const QString &getSummary() const;

    const QUrl &getWebsiteUrl() const;

    const QList<ModrinthFileInfo> &getFileList() const;

    const QList<GameVersion> &getGameVersions() const;

    const QStringList &getVersionList() const;

private:
    QString id;
    QString author;
    QString name;
    QString summary;
    QString description;
    QList<GameVersion> gameVersions;
    int downloadCount;
    QUrl websiteUrl;
    QUrl iconUrl;
    QByteArray iconBytes;

    QStringList versionList;
    QList<ModrinthFileInfo> fileList;

    bool basicInfo = false;
    bool fullInfo = false;
};

#endif // MODRINTHMODINFO_H
