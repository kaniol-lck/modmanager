#include "curseforgeapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"

const QString CurseforgeAPI::PREFIX = "https://addons-ecs.forgesvc.net";

// SEARCH            = "/api/v2/addon/search";
// FINGERPRINT       = "/api/v2/fingerprint";
// DESCRIPTION       = "/api/v2/addon/{addonID}/description";
// CHANGELOG         = "/api/v2/addon/{addonID}/file/{fileID}/changelog";
// DOWNLOAD_URL      = "/api/v2/addon/{addonID}/file/{fileID}/download-url";
// FILE_INFO         = "/api/v2/addon/{addonID}/file/{fileID}";
// FILES             = "/api/v2/addon/{addonID}/files";
// INFO              = "/api/v2/addon/{addonID}";
// TIMESTAMP         = "/api/v2/addon/timestamp";
// MINECRAFT_VERSION = "/api/v2/minecraft/version";

CurseforgeAPI *CurseforgeAPI::api()
{
    static CurseforgeAPI api;
    return &api;
}

void CurseforgeAPI::searchMods(const GameVersion &version, int index, const QString &searchFilter, int sort, std::function<void (QList<CurseforgeModInfo>)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/search";

    //url query
    QUrlQuery urlQuery;

    //?
    urlQuery.addQueryItem("categoryId", "0");
    //minecraft
    urlQuery.addQueryItem("gameId", "432");
    //game version
    if(version != GameVersion::Any)
        urlQuery.addQueryItem("gameVersion", version);
    //index
    urlQuery.addQueryItem("index", QString::number(index));
    //search page size, 30 by default [Customize it]
    urlQuery.addQueryItem("pageSize", "30");
    //search by name
    urlQuery.addQueryItem("searchFilter", searchFilter);
    //mod
    urlQuery.addQueryItem("sectionId", "6");
    //sort, 0 for no sort spec
    urlQuery.addQueryItem("sort", QString::number(sort));

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
        auto resultList = jsonDocument.toVariant().toList();
        QList<CurseforgeModInfo> modInfoList;

        for(const auto &result : qAsConst(resultList)){
            auto modInfo = CurseforgeModInfo::fromVariant(result);
            modInfoList.append(modInfo);
        }
        callback(modInfoList);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getIdByFingerprint(const QString &fingerprint, std::function<void (int, CurseforgeFileInfo, QList<CurseforgeFileInfo>)> callback, std::function<void ()> noMatch)
{
    QUrl url = PREFIX + "/api/v2/fingerprint";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto reply = api()->accessManager.post(request, QString("[ %1 ]").arg(fingerprint).toUtf8());
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
        auto exactMatchList = value(jsonDocument.toVariant(), "exactMatches").toList();
        if(exactMatchList.isEmpty())
            noMatch();
        else {
            int id = value(exactMatchList.at(0), "id").toInt();
            auto file = CurseforgeFileInfo::fromVariant(value(exactMatchList.at(0), "file"));
            auto latestFileList = value(exactMatchList.at(0), "latestFiles").toList();
            QList<CurseforgeFileInfo> fileList;
            for (const auto &variant : latestFileList)
                fileList << CurseforgeFileInfo::fromVariant(variant);

            callback(id, file, fileList);
        }
        reply->deleteLater();
    });
}

void CurseforgeAPI::getDescription(int id, std::function<void (QString)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/description";

    QNetworkRequest request(url);
    auto reply = api()->accessManager.get(request);
    connect(reply, &QNetworkReply::finished, api(), [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        auto description = reply->readAll();
        callback(description);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getFiles(int id, std::function<void (QList<CurseforgeFileInfo>)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/files";

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
        auto resultList = jsonDocument.toVariant().toList();
        QList<CurseforgeFileInfo> fileInfoList;

        for(const auto &result : qAsConst(resultList)){
            auto modInfo = CurseforgeFileInfo::fromVariant(result);
            fileInfoList.append(modInfo);
        }

        callback(fileInfoList);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getInfo(int id, std::function<void (CurseforgeModInfo)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id);

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

        auto modInfo = CurseforgeModInfo::fromVariant(result);

        callback(modInfo);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getMinecraftVersionList(std::function<void (QList<GameVersion>)> callback)
{
    QUrl url = PREFIX + "/api/v2/minecraft/version";

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
        auto list = jsonDocument.toVariant().toList();
        QList<GameVersion> versionList;
        for(const auto &entry : qAsConst(list))
            versionList << value(entry, "versionString").toString();

        callback(versionList);
        reply->deleteLater();
    });
}

CurseforgeAPI::CurseforgeAPI(QObject *parent) : QObject(parent)
{

}
