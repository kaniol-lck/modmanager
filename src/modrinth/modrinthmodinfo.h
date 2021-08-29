#ifndef MODRINTHMODINFO_H
#define MODRINTHMODINFO_H

#include <QVariant>
#include <QUrl>

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

private:
    QString id;
    QString author;
    QString name;
    QString summary;
    QString description;
    QStringList versions;
    int downloadCount;
    QUrl iconUrl;
    QByteArray iconBytes;

    bool basicInfo = false;
    bool fullInfo = false;
};

#endif // MODRINTHMODINFO_H
