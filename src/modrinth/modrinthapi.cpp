#include "modrinthapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>

#include "util/tutil.hpp"
#include "util/mmlogger.h"
#include "config.hpp"
#include "version.h"

namespace {
QByteArray modrinthUserAgent()
{
    QByteArray version(kVersion);
    if(version.startsWith('v'))
        version.remove(0, 1);
    return "kaniol-lck/modmanager/" + version + " (+https://github.com/kaniol-lck/modmanager)";
}
}

const QString ModrinthAPI::PREFIX = "https://api.modrinth.com";
const QByteArray ModrinthAPI::USER_AGENT = modrinthUserAgent();

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

QNetworkRequest ModrinthAPI::createRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    return request;
}

Reply<QList<ModrinthModInfo>> ModrinthAPI::searchMods(const QString name, int index, const QList<GameVersion> &versions, ModLoaderType::Type type, const QList<QString> &categories, int sort)
{
    QUrl url = PREFIX + "/v2/search";

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
        str = "follows";
        break;
    case 3:
        str = "newest";
        break;
    case 4:
        str = "updated";
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

    //category
    for(const auto &category : categories)
        if(!category.isEmpty())
            categoryFacets << "categories:" + category;

    if(!categoryFacets.isEmpty())
        facets << categoryFacets;

    if(!facets.isEmpty())
        urlQuery.addQueryItem("facets", QJsonDocument(facets).toJson(QJsonDocument::Compact));

    url.setQuery(urlQuery);
    auto request = createRequest(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply,  [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if(error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QList<ModrinthModInfo>{};
            }

            auto resultList = value(jsonDocument.toVariant(), "hits").toList();
            QList<ModrinthModInfo> modInfoList;
            for(const auto &result : qAsConst(resultList))
                modInfoList << ModrinthModInfo::fromSearchVariant(result);

            return modInfoList;
        } };
}

Reply<ModrinthModInfo> ModrinthAPI::getInfo(const QString &id)
{
    //id: "local-xxxxx" ???
    auto modId = id.startsWith("local-")? id.right(id.size() - 6) : id;
    QUrl url = PREFIX + "/v2/project/" + modId;
    auto request = createRequest(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply,  [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return ModrinthModInfo();
            }

            auto result = jsonDocument.toVariant();
            auto modrinthModInfo = ModrinthModInfo::fromVariant(result);

            return modrinthModInfo;
        } };
}

Reply<QList<ModrinthFileInfo> > ModrinthAPI::getVersions(const QString &id)
{
    //id: "local-xxxxx" ???
    auto modId = id.startsWith("local-")? id.right(id.size() - 6) : id;
    QUrl url = PREFIX + "/v2/project/" + modId + "/version";

    auto request = createRequest(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QList<ModrinthFileInfo>{};
            }

            auto resultList = jsonDocument.toVariant().toList();
            QList<ModrinthFileInfo> fileInfoList;
            for(const auto &result : qAsConst(resultList))
                fileInfoList << ModrinthFileInfo::fromVariant(result);
            return fileInfoList;
        } };
}

Reply<ModrinthFileInfo> ModrinthAPI::getVersion(const QString &version)
{
    QUrl url = PREFIX + "/v2/version/" + version;
    auto request = createRequest(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return ModrinthFileInfo();
            }

            auto result = jsonDocument.toVariant();
            return ModrinthFileInfo::fromVariant(result);
        } };
}

Reply<QString> ModrinthAPI::getAuthor(const QString &authorId)
{
    QUrl url = PREFIX + "/v2/user/" + authorId;
    auto request = createRequest(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return QString();
            }

            auto result = jsonDocument.toVariant();
            return value(result, "name").toString();
        } };
}

Reply<ModrinthFileInfo> ModrinthAPI::getVersionFileBySha1(const QString sha1)
{
    QUrl url = PREFIX + "/v2/version_file/" + sha1;

    //url query
    QUrlQuery urlQuery;
    //set type sha1
    urlQuery.addQueryItem("algorithm", "sha1");

    url.setQuery(urlQuery);
    auto request = createRequest(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
            //parse json
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug("%s", error.errorString().toUtf8().constData());
                return ModrinthFileInfo();
            }
            auto result = jsonDocument.toVariant();
            return ModrinthFileInfo::fromVariant(result);
        } };
}

const QList<std::tuple<QString, QString> > &ModrinthAPI::getCategories()
{
    static const QList<std::tuple<QString, QString>> categories{
        { tr("Adventure"), "adventure" },
        { tr("Cursed"), "cursed" },
        { tr("Decoration"), "decoration" },
        { tr("Economy"), "economy" },
        { tr("Equipment"), "equipment" },
        { tr("Food"), "food" },
        { tr("Game Mechanics"), "game-mechanics" },
        { tr("Library"), "library" },
        { tr("Magic"), "magic" },
        { tr("Management"), "management" },
        { tr("Minigame"), "minigame" },
        { tr("Mobs"), "mobs" },
        { tr("Optimization"), "optimization" },
        { tr("Social"), "social" },
        { tr("Storage"), "storage" },
        { tr("Technology"), "technology" },
        { tr("Transportation"), "transportation" },
        { tr("Utility"), "utility" },
        { tr("World generation"), "worldgen" },
    };
    return categories;
}
