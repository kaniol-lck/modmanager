#ifndef MODRINTHAPI_H
#define MODRINTHAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "modrinthmodinfo.h"
#include "modrinthfileinfo.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "network/reply.hpp"

class ModrinthAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;

public:
    explicit ModrinthAPI(QObject *parent = nullptr);
    static ModrinthAPI *api();

    Reply<QList<ModrinthModInfo>> searchMods(const QString name, int index, const QList<GameVersion> &versions, ModLoaderType::Type type, const QList<QString> &categories, int sort);
    Reply<ModrinthModInfo> getInfo(const QString &id);
    Reply<QList<ModrinthFileInfo>> getVersions(const QString &id);
    Reply<ModrinthFileInfo> getVersion(const QString &version);
    Reply<QString> getAuthor(const QString &authorId);
    Reply<ModrinthFileInfo> getVersionFileBySha1(const QString sha1);

    static const QList<std::tuple<QString, QString>> &getCategories();
private:
    QNetworkAccessManager accessManager_;
};

#endif // MODRINTHAPI_H
