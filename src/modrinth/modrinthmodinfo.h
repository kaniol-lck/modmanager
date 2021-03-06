#ifndef MODRINTHMODINFO_H
#define MODRINTHMODINFO_H

#include <QVariant>
#include <QUrl>

#include "tag/tagable.h"

#include "modrinthfileinfo.h"

class ModrinthModInfo : Tagable
{
    friend class ModrinthMod;
public:
    ModrinthModInfo() = default;
    explicit ModrinthModInfo(const QString &id);

    static const QString &cachePath();

    static ModrinthModInfo fromSearchVariant(const QVariant &variant);
    static ModrinthModInfo fromVariant(const QVariant &variant);

    const QString &id() const;
    const QString &author() const;
    const QString &authorId() const;
    const QString &name() const;
    const QString &description() const;
    const QStringList &versions() const;
    int downloadCount() const;
    const QList<ModLoaderType::Type> &loaderTypes() const;
    const QPixmap &icon() const;
    const QUrl &iconUrl() const;
    const QString &summary() const;
    const QUrl &websiteUrl() const;
    const QList<ModrinthFileInfo> &fileList() const;
    QList<ModrinthFileInfo> featuredFileList() const;
    const QList<GameVersion> &gameVersions() const;
    const QStringList &categories() const;
    const QStringList &versionList() const;
    const QDateTime &dateCreated() const;
    const QDateTime &dateModified() const;
    bool hasBasicInfo() const;
    bool hasFullInfo() const;

private:
    QString modId_;
    QString author_;
    QString authorId_;
    QString name_;
    QString summary_;
    QString description_;
    QList<GameVersion> gameVersions_;
    int downloadCount_;
    QList<ModLoaderType::Type> loaderTypes_;
    QUrl websiteUrl_;
    QUrl iconUrl_;
    QPixmap icon_;
    QStringList categories_;
    QDateTime dateCreated_;
    QDateTime dateModified_;

    QStringList versionList_;
    QList<ModrinthFileInfo> fileList_;

    bool basicInfo_ = false;
    bool fullInfo_ = false;
};

#endif // MODRINTHMODINFO_H
