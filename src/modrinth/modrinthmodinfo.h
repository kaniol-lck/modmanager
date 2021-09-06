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
    explicit ModrinthModInfo(const QString &id);

    static ModrinthModInfo fromSearchVariant(const QVariant &variant);
    static ModrinthModInfo fromVariant(const QVariant &variant);

    const QString &id() const;

    const QString &author() const;

    const QString &name() const;

    const QString &description() const;

    const QStringList &versions() const;

    int downloadCount() const;

    const QUrl &iconUrl() const;

    const QByteArray &iconBytes() const;

    const QString &summary() const;

    const QUrl &websiteUrl() const;

    const QList<ModrinthFileInfo> &fileList() const;
    QList<ModrinthFileInfo> featuredFileList() const;

    const QList<GameVersion> &gameVersions() const;

    const QStringList &versionList() const;

    bool hasBasicInfo() const;

    bool hasFullInfo() const;

private:
    QString modId_;
    QString author_;
    QString name_;
    QString summary_;
    QString description_;
    QList<GameVersion> gameVersions_;
    int downloadCount_;
    QUrl websiteUrl_;
    QUrl iconUrl_;
    QByteArray iconBytes_;

    QStringList versionList_;
    QList<ModrinthFileInfo> fileList_;

    bool basicInfo_ = false;
    bool fullInfo_ = false;
};

#endif // MODRINTHMODINFO_H
