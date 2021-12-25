#include "modrinthapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>

#include "util/tutil.hpp"
#include "util/mmlogger.h"
#include "config.hpp"

const QString ModrinthAPI::PREFIX = "https://api.modrinth.com";

ModrinthAPI::ModrinthAPI(QObject *parent) :
    QObject(parent)
{
    accessManager_.setTransferTimeout(Config().getNetworkRequestTimeout());
}

ModrinthAPI *ModrinthAPI::api()
{
    static ModrinthAPI api;
    return &api;
}

QMetaObject::Connection ModrinthAPI::searchMods(const QString name, int index, const QList<GameVersion> &versions, ModLoaderType::Type type, const QList<QString> &categories, int sort, std::function<void (QList<ModrinthModInfo>)> callback)
{
    QUrl url = PREFIX + "/api/v1/mod";

    //url query
    QUrlQuery urlQuery;

    //name
    urlQuery.addQueryItem("query", name);
    //sort
    QString str;
    switch (sort) {
    case 0:
        str = "relevance";
        break;
    case 1:
        str = "downloads";
        break;
    case 2:
        str = "updated";
        break;
    case 3:
        str = "newest";
        break;
    }
    urlQuery.addQueryItem("index", str);
    //index - offset
    urlQuery.addQueryItem("offset", QString::number(index));
    //search page size
    urlQuery.addQueryItem("limit", QString::number(Config().getSearchResultCount()));

    QJsonArray facets;
    //game version
    QJsonArray versionFacets;
    for(const auto &version : versions)
        if(version != GameVersion::Any)
            versionFacets << "versions:" + version;
    if(!versionFacets.isEmpty())
        facets << versionFacets;

    //loader type
    QJsonArray categoryFacets;
    if(type != ModLoaderType::Any)
        categoryFacets << "categories:" + ModLoaderType::toString(type);

    //loader type
    for(const auto &category : categories)
        if(!category.isEmpty())
            categoryFacets << "categories:" + category;

    if(!categoryFacets.isEmpty())
        facets << categoryFacets;

    if(!facets.isEmpty())
        urlQuery.addQueryItem("facets", QJsonDocument(facets).toJson(QJsonDocument::Compact));

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this,  [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if(error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }

        auto resultList = value(jsonDocument.toVariant(), "hits").toList();

        QList<ModrinthModInfo> modInfoList;
        for(const auto &result : qAsConst(resultList))
            modInfoList << ModrinthModInfo::fromSearchVariant(result);

        callback(modInfoList);
        reply->deleteLater();
    });
}

QMetaObject::Connection ModrinthAPI::getInfo(const QString &id, std::function<void (ModrinthModInfo)> callback)
{
    //id: "local-xxxxx" ???
    auto modId = id.startsWith("local-")? id.right(id.size() - 6) : id;
    QUrl url = PREFIX + "/api/v1/mod/" + modId;
    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this,  [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            callback({});
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }

        auto result = jsonDocument.toVariant();
        auto modrinthModInfo = ModrinthModInfo::fromVariant(result);

        callback(modrinthModInfo);
        reply->deleteLater();
    });
}

QMetaObject::Connection ModrinthAPI::getVersions(const QString &id, std::function<void (QList<ModrinthFileInfo>)> callback, std::function<void ()> failed)
{
    //id: "local-xxxxx" ???
    auto modId = id.startsWith("local-")? id.right(id.size() - 6) : id;
    QUrl url = PREFIX + "/api/v1/mod/" + modId + "/version";

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this,  [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            failed();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }

        auto resultList = jsonDocument.toVariant().toList();

        QList<ModrinthFileInfo> fileInfoList;
        for(const auto &result : qAsConst(resultList))
            fileInfoList << ModrinthFileInfo::fromVariant(result);

        callback(fileInfoList);
        reply->deleteLater();
    });
}

QMetaObject::Connection ModrinthAPI::getVersion(const QString &version, std::function<void (ModrinthFileInfo)> callback)
{
    QUrl url = PREFIX + "/api/v1/version/" + version;
    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this,  [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }

        auto result = jsonDocument.toVariant();
        auto modrinthFileInfo = ModrinthFileInfo::fromVariant(result);

        callback(modrinthFileInfo);
        reply->deleteLater();
    });
}

QMetaObject::Connection ModrinthAPI::getAuthor(const QString &authorId, std::function<void (QString)> callback)
{
    QUrl url = PREFIX + "/api/v1/user/" + authorId;
    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this,  [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }

        auto result = jsonDocument.toVariant();
        auto author = value(result, "name").toString();

        callback(author);
        reply->deleteLater();
    });
}

QMetaObject::Connection ModrinthAPI::getVersionFileBySha1(const QString sha1, std::function<void (ModrinthFileInfo)> callback, std::function<void ()> noMatch)
{
    QUrl url = PREFIX + "/api/v1/version_file/" + sha1;

    //url query
    QUrlQuery urlQuery;
    //set type sha1
    urlQuery.addQueryItem("algorithm", "sha1");

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this,  [=]{
        if(reply->error() != QNetworkReply::NoError) {
            if(reply->error() == QNetworkReply::ContentNotFoundError)
                noMatch();
            else
                qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }
        auto result = jsonDocument.toVariant();

        auto modrinthFileInfo = ModrinthFileInfo::fromVariant(result);
        callback(modrinthFileInfo);
        reply->deleteLater();
    });
}

const QList<std::tuple<QString, QString> > &ModrinthAPI::getCategories()
{
    static const QList<std::tuple<QString, QString>> categories{
        { tr("World generation"), "worldgen" },
        { tr("Technology"), "technology" },
        { tr("Food"), "food" },
        { tr("Magic"), "magic" },
        { tr("Storage"), "storage" },
        { tr("Library"), "library" },
        { tr("Adventure"), "adventure" },
        { tr("Utility"), "utility" },
        { tr("Decoration"), "decoration" },
        { tr("Miscellaneous"), "misc" },
        { tr("Equipment"), "equipment" },
        { tr("Cursed"), "cursed" }
    };
    return categories;
}
