#ifndef MODLINKCACHE_H
#define MODLINKCACHE_H

#include <QVariant>

class LocalMod;
class QJsonObject;

class ModLinkCache
{
public:
    ModLinkCache(const QString &path);

    void addCache(LocalMod *localMod);

    void saveToFile();

    class ModLink
    {
        friend class ModLinkCache;
    public:
        QJsonObject toJsonObject();
        static ModLink fromVariant(const QVariant &variant);

        const QString &id() const;
        int curseforgeId() const;
        int curseforgeFileId() const;
        const QString &modrinthId() const;
        const QString &modrinthFileId() const;

    private:
        QString id_;
        int curseforgeId_ = 0;
        int curseforgeFileId_ = 0;
        QString modrinthId_;
        QString modrinthFileId_;
    };
private:
    constexpr static auto FileName = "links.json";
    QString path_;
    QMap<QString, ModLink> linkCaches_;
};

#endif // MODLINKCACHE_H
