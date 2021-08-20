#ifndef CURSEFORGEMODINFO_H
#define CURSEFORGEMODINFO_H

#include <QVariant>
#include <QUrl>
#include <optional>
#include <modloadertype.h>

#include "curseforgefileinfo.h"
#include "modloadertype.h"

class CurseforgeMod;


class CurseforgeModInfo
{
    friend class CurseforgeMod;
public:
    CurseforgeModInfo();

    static CurseforgeModInfo fromVariant(const QVariant &variant);

    int getId() const;

    const QString &getName() const;

    const QString &getSummary() const;

    const QUrl &getWebsiteUrl() const;

    const QStringList &getAuthors() const;

    const QUrl &getThumbnailUrl() const;

    const QByteArray &getThumbnailBytes() const;

    const QString &getDescription() const;

    int getDownloadCount() const;

    const QList<ModLoaderType::Type> &getModLoaders() const;

    bool isFabricMod() const;

    bool isForgeMod() const;

    bool isRiftMod() const;

    const QList<CurseforgeFileInfo> &getFileInfoList() const;

    std::optional<CurseforgeFileInfo> getFileInfo(const GameVersion &version, ModLoaderType::Type &loaderType) const;

private:
    int id;
    QString name;
    QString summary;
    QUrl websiteUrl;
    QStringList authors;
    QUrl thumbnailUrl;
    QByteArray thumbnailBytes;
    QString description;
    int downloadCount;
    QList<ModLoaderType::Type> modLoaders;
    QList<CurseforgeFileInfo> fileInfoList;
};

#endif // CURSEFORGEMODINFO_H
