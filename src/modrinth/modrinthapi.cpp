#include "modrinthapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"

const QString ModrinthAPI::PREFIX = "https://api.modrinth.com";

ModrinthAPI *ModrinthAPI::api()
{
    static ModrinthAPI api;
    return &api;
}

void ModrinthAPI::searchMods(const QString name, int index, const GameVersion &version, ModLoaderType::Type type, int sort, std::function<void (QList<ModrinthModInfo>)> callback)
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
    //search page size, 30 by default [Customize it]
    urlQuery.addQueryItem("limit", "30");

    QStringList facets;
    //game version
    if(version != GameVersion::Any)
        facets << "\"versions:" + version + "\"";

    //loader type
    if(type != ModLoaderType::Any)
        facets << "\"categories:" + ModLoaderType::toString(type) + "\"";

    if(!facets.isEmpty())
        urlQuery.addQueryItem("facets", "[[" + facets.join(",") + "]]");

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto reply = api()->accessManager.get(request);
    connect(reply, &QNetworkReply::finished, api(), [=]{
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
        auto resultList = value(jsonDocument.toVariant(), "hits").toList();
        QList<ModrinthModInfo> modInfoList;

        for(const auto &result : qAsConst(resultList)){
            auto modrinthModInfo = ModrinthModInfo::fromSearchVariant(result);
            modInfoList.append(modrinthModInfo);
        }
        callback(modInfoList);
        reply->deleteLater();
    });
}

void ModrinthAPI::getInfo(const QString &id, std::function<void (ModrinthModInfo)> callback)
{
    //id: "local-xxxxx" ???
    auto modId = id.startsWith("local-")? id.right(id.size() - 6) : id;
    QUrl url = PREFIX + "/api/v1/mod/" + modId;
    QNetworkRequest request(url);
    auto reply = api()->accessManager.get(request);
    connect(reply, &QNetworkReply::finished, api(), [=]{
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
        auto modrinthModInfo = ModrinthModInfo::fromVariant(result);

        callback(modrinthModInfo);
        reply->deleteLater();
    });

}

void ModrinthAPI::getVersion(const QString &version, std::function<void (ModrinthFileInfo)> callback)
{
    QUrl url = PREFIX + "/api/v1/version/" + version;
    QNetworkRequest request(url);
    auto reply = api()->accessManager.get(request);
    connect(reply, &QNetworkReply::finished, api(), [=]{
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

void ModrinthAPI::getVersionFileBySha1(const QString sha1, std::function<void (ModrinthFileInfo)> callback, std::function<void ()> noMatch)
{
    QUrl url = PREFIX + "/api/v1/version_file/" + sha1;

    //url query
    QUrlQuery urlQuery;
    //set type sha1
    urlQuery.addQueryItem("algorithm", "sha1");

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto reply = api()->accessManager.get(request);
    connect(reply, &QNetworkReply::finished, api(), [=]{
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

ModrinthAPI::ModrinthAPI(QObject *parent) :
    QObject(parent)
{

}
