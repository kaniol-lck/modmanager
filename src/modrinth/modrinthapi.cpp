#include "modrinthapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "config.h"

const QString ModrinthAPI::PREFIX = "https://api.modrinth.com";

ModrinthAPI::ModrinthAPI(QObject *parent) :
    QObject(parent)
{}

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
    //search page size
    urlQuery.addQueryItem("limit", QString::number(Config().getSearchResultCount()));

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
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this,  [=]{
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
        for(const auto &result : qAsConst(resultList))
            modInfoList << ModrinthModInfo::fromSearchVariant(result);

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
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this,  [=]{
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

void ModrinthAPI::getVersions(const QString &id, std::function<void (QList<ModrinthFileInfo>)> callback)
{
    //id: "local-xxxxx" ???
    auto modId = id.startsWith("local-")? id.right(id.size() - 6) : id;
    QUrl url = PREFIX + "/api/v1/mod/" + modId + "/version";

    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this,  [=]{
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

        auto resultList = jsonDocument.toVariant().toList();

        QList<ModrinthFileInfo> fileInfoList;
        for(const auto &result : qAsConst(resultList))
            fileInfoList << ModrinthFileInfo::fromVariant(result);

        callback(fileInfoList);
        reply->deleteLater();
    });
}

void ModrinthAPI::getVersion(const QString &version, std::function<void (ModrinthFileInfo)> callback)
{
    QUrl url = PREFIX + "/api/v1/version/" + version;
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this,  [=]{
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

void ModrinthAPI::getAuthor(const QString &authorId, std::function<void (QString)> callback)
{
    QUrl url = PREFIX + "/api/v1/user/" + authorId;
    qDebug() << url;
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this,  [=]{
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

void ModrinthAPI::getVersionFileBySha1(const QString sha1, std::function<void (ModrinthFileInfo)> callback, std::function<void ()> noMatch)
{
    QUrl url = PREFIX + "/api/v1/version_file/" + sha1;

    //url query
    QUrlQuery urlQuery;
    //set type sha1
    urlQuery.addQueryItem("algorithm", "sha1");

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this,  [=]{
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
