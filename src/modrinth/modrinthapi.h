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
    explicit ModrinthAPI(QObject *parent = nullptr);
    static ModrinthAPI *api();

    [[nodiscard]] QMetaObject::Connection searchMods(const QString name, int index, const QList<GameVersion> &versions, ModLoaderType::Type type, const QList<QString> &categories, int sort, std::function<void (QList<ModrinthModInfo>)> callback);
    [[nodiscard]] QMetaObject::Connection getInfo(const QString &id, std::function<void (ModrinthModInfo)> callback);
    [[nodiscard]] QMetaObject::Connection getVersions(const QString &id, std::function<void (QList<ModrinthFileInfo>)> callback, std::function<void ()> failed = []{});
    [[nodiscard]] QMetaObject::Connection getVersion(const QString &version, std::function<void (ModrinthFileInfo)> callback);
    [[nodiscard]] QMetaObject::Connection getAuthor(const QString &authorId, std::function<void (QString)> callback);
    [[nodiscard]] QMetaObject::Connection getVersionFileBySha1(const QString sha1, std::function<void (ModrinthFileInfo)> callback, std::function<void ()> noMatch);

    static const QList<std::tuple<QString, QString>> &getCategories();
private:
    QNetworkAccessManager accessManager_;
};

#endif // MODRINTHAPI_H
