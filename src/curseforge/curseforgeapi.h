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
    static CurseforgeAPI *api();

    static void searchMods(const GameVersion &version, int index, const QString &searchFilter, int sort, std::function<void (QList<CurseforgeModInfo>)> callback);
    static void getIdByFingerprint(const QString &fingerprint, std::function<void (int, CurseforgeFileInfo, QList<CurseforgeFileInfo>)> callback, std::function<void()> noMatch);
    static void getDescription(int id, std::function<void(QString)> callback);
    static void getChangelog(int id, int FileID, std::function<void (QString)> callback);
    static void getDownloadUrl(int id, int FileID, std::function<void (QString)> callback);
//    static void getFileInfo(int id, int FileID, std::function<void(CurseforgeFileInfo)> callback);
    static void getFiles(int id, std::function<void (QList<CurseforgeFileInfo>)> callback);
    static void getInfo(int id, std::function<void (CurseforgeModInfo)> callback);
    static void getTimestamp(std::function<void (QString)> callback);
    static void getMinecraftVersionList(std::function<void (QList<GameVersion>)> callback);

private:
    explicit CurseforgeAPI(QObject *parent = nullptr);
    QNetworkAccessManager accessManager;

};

#endif // CURSEFORGEAPI_H
