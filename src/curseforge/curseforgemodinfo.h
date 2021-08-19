#ifndef CURSEFORGEMODINFO_H
#define CURSEFORGEMODINFO_H

#include <QVariant>
#include <QUrl>

class CurseforgeMod;


class CurseforgeModInfo
{
    friend class CurseforgeMod;
public:
    CurseforgeModInfo();

    static CurseforgeModInfo fromVariant(const QVariant &variant);

    static CurseforgeModInfo fromGetInfo(const QVariant &variant);

    int getId() const;

    const QString &getName() const;

    const QString &getSummary() const;

    const QUrl &getWebsiteUrl() const;

    const QStringList &getAuthors() const;

    const QUrl &getThumbnailUrl() const;

    const QByteArray &getThumbnailBytes() const;

    const QUrl &getLatestFileUrl() const;

    const QString &getLatestFileName() const;

    int getLatestFileLength() const;

    const QString &getDescription() const;

    int getDownloadCount() const;

    const QStringList &getModLoaders() const;

    bool isFabricMod() const;

    bool isForgeMod() const;

    bool isRiftMod() const;

private:
    int id;
    QString name;
    QString summary;
    QUrl websiteUrl;
    QStringList authors;
    QUrl thumbnailUrl;
    QByteArray thumbnailBytes;
    QUrl latestFileUrl;
    QString latestFileName;
    int latestFileLength;
    QString description;
    int downloadCount;
    QStringList modLoaders;
};

#endif // CURSEFORGEMODINFO_H
