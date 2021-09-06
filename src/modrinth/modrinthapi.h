#ifndef MODRINTHAPI_H
#define MODRINTHAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "modrinthmodinfo.h"
#include "modrinthfileinfo.h"
#include "gameversion.h"
#include "modloadertype.h"

class ModrinthAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;
public:
    static ModrinthAPI *api();

    static void searchMods(const QString name, int index, const GameVersion &version, ModLoaderType::Type type, int sort, std::function<void (QList<ModrinthModInfo>)> callback);
    static void getInfo(const QString &id, std::function<void (ModrinthModInfo)> callback);
    static void getVersion(const QString &version, std::function<void (ModrinthFileInfo)> callback);
    static void getVersionFileBySha1(const QString sha1, std::function<void (ModrinthFileInfo)> callback, std::function<void ()> noMatch);

private:
    explicit ModrinthAPI(QObject *parent = nullptr);
    QNetworkAccessManager accessManager;
};

#endif // MODRINTHAPI_H
