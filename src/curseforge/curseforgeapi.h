#ifndef CURSEFORGEAPI_H
#define CURSEFORGEAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>
#include <QNetworkDiskCache>

#include "gameversion.h"
#include "curseforgemodinfo.h"
#include "curseforgecategoryinfo.h"
#include "network/reply.hpp"

class CurseforgeAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;
    static const QByteArray XAPIKEY;

public:
    enum Section { BukkitPlugin = 5/*?*/, Mod = 6, TexturePack = 12, World = 17, Modpack = 4471, Addon = 4559/*?*/, Customization = 4546/*?*/ };
    explicit CurseforgeAPI(QObject *parent = nullptr);
    static CurseforgeAPI *api();

    Reply<QList<CurseforgeModInfo> > searchMods(int sectionId, const GameVersion &version, int index, const QString &searchFilter, int category, int sort);
    Reply<int, CurseforgeFileInfo, QList<CurseforgeFileInfo> > getIdByFingerprint(const QString &fingerprint);
    Reply<QString> getDescription(int id);
    Reply<QString> getChangelog(int id, int FileID);
    Reply<QString> getDownloadUrl(int id, int FileID);
    Reply<CurseforgeFileInfo> getFileInfo(int id, int FileID);
    Reply<QList<CurseforgeFileInfo> > getFiles(int id);
    Reply<CurseforgeModInfo> getInfo(int id);
    Reply<QString> getTimestamp();
    Reply<QList<GameVersion> > getMinecraftVersionList();
    Reply<QList<CurseforgeCategoryInfo> > getSectionCategories(int sectionId);
    static QList<CurseforgeCategoryInfo> cachedSectionCategories(int sectionId);

private:
    QNetworkAccessManager accessManager_;
    QNetworkDiskCache diskCache_;

    static QString cachedCategoriesFilePath(int sectionId);
};

#endif // CURSEFORGEAPI_H
