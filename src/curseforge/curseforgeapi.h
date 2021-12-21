#ifndef CURSEFORGEAPI_H
#define CURSEFORGEAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "gameversion.h"
#include "curseforgemodinfo.h"

class CurseforgeAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;

public:
    enum Section { BukkitPlugin = 5/*?*/, Mod = 6, TexturePack = 12, World = 17, Modpack = 4471, Addon = 4559/*?*/, Customization = 4546/*?*/ };
    explicit CurseforgeAPI(QObject *parent = nullptr);
    static CurseforgeAPI *api();

    [[nodiscard]] QMetaObject::Connection searchMods(int sectionId, const GameVersion &version, int index, const QString &searchFilter, int category, int sort, std::function<void (QList<CurseforgeModInfo>)> callback);
    [[nodiscard]] QMetaObject::Connection getIdByFingerprint(const QString &fingerprint, std::function<void (int, CurseforgeFileInfo, QList<CurseforgeFileInfo>)> callback, std::function<void()> noMatch);
    [[nodiscard]] QMetaObject::Connection getDescription(int id, std::function<void(QString)> callback);
    [[nodiscard]] QMetaObject::Connection getChangelog(int id, int FileID, std::function<void (QString)> callback);
    [[nodiscard]] QMetaObject::Connection getDownloadUrl(int id, int FileID, std::function<void (QString)> callback);
    [[nodiscard]] QMetaObject::Connection getFileInfo(int id, int FileID, std::function<void(CurseforgeFileInfo)> callback);
    [[nodiscard]] QMetaObject::Connection getFiles(int id, std::function<void (QList<CurseforgeFileInfo>)> callback, std::function<void ()> failed = []{});
    [[nodiscard]] QMetaObject::Connection getInfo(int id, std::function<void (CurseforgeModInfo)> callback);
    [[nodiscard]] QMetaObject::Connection getTimestamp(std::function<void (QString)> callback);
    [[nodiscard]] QMetaObject::Connection getMinecraftVersionList(std::function<void (QList<GameVersion>)> callback);

    static const QList<std::tuple<int, QString, QString, int> > &getCategories();

private:
    QNetworkAccessManager accessManager_;
};

#endif // CURSEFORGEAPI_H
