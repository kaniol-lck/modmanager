#include "curseforgeapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "config.hpp"

const QString CurseforgeAPI::PREFIX = "https://addons-ecs.forgesvc.net";

CurseforgeAPI::CurseforgeAPI(QObject *parent) :
    QObject(parent)
{}

CurseforgeAPI *CurseforgeAPI::api()
{
    static CurseforgeAPI api;
    return &api;
}

void CurseforgeAPI::searchMods(const GameVersion &version, int index, const QString &searchFilter, int category, int sort, std::function<void (QList<CurseforgeModInfo>)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/search";

    //url query
    QUrlQuery urlQuery;

    //?
    urlQuery.addQueryItem("categoryId", QString::number(category));
    //minecraft
    urlQuery.addQueryItem("gameId", "432");
    //game version
    if(version != GameVersion::Any)
        urlQuery.addQueryItem("gameVersion", version);
    //index
    urlQuery.addQueryItem("index", QString::number(index));
    //search page size
    urlQuery.addQueryItem("pageSize", QString::number(Config().getSearchResultCount()));
    //search by name
    urlQuery.addQueryItem("searchFilter", searchFilter);
    //mod
    urlQuery.addQueryItem("sectionId", "6");
    //sort, 0 for no sort spec
    urlQuery.addQueryItem("sort", QString::number(sort + 1));

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
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

        for(const auto &result : qAsConst(resultList))
            modInfoList << CurseforgeModInfo::fromVariant(result);

        callback(modInfoList);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getIdByFingerprint(const QString &fingerprint, std::function<void (int, CurseforgeFileInfo, QList<CurseforgeFileInfo>)> callback, std::function<void ()> noMatch)
{
    QUrl url = PREFIX + "/api/v2/fingerprint";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto reply = accessManager_.post(request, QString("[ %1 ]").arg(fingerprint).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [=]{
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
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        auto description = reply->readAll();
        callback(description);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getFileInfo(int id, int FileID, std::function<void (CurseforgeFileInfo)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/file/" + QString::number(FileID);

    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
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

        auto fileInfo = CurseforgeFileInfo::fromVariant(result);
        callback(fileInfo);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getFiles(int id, std::function<void (QList<CurseforgeFileInfo>)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/files";

    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
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
        for(const auto &result : qAsConst(resultList))
            fileInfoList << CurseforgeFileInfo::fromVariant(result);

        callback(fileInfoList);
        reply->deleteLater();
    });
}

void CurseforgeAPI::getInfo(int id, std::function<void (CurseforgeModInfo)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id);

    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
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
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
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

const QList<std::tuple<int, QString, QString> > &CurseforgeAPI::getCategories()
{
    static const QList<std::tuple<int, QString, QString>> categories{
        { 423, tr("Map and Information"), "map-information" },
        { 426, tr("Addons"), "mc-addons" },
        { 434, tr("Armor, Tools, and Weapons"), "armor-weapons-tools" },
        { 409, tr("Structures"), "world-structures" },
        { 4485, tr("Blood Magic"), "blood-magic" },
        { 429, tr("Industrial Craft"), "addons-industrialcraft" },
        { 420, tr("Storage"), "storage" },
        { 428, tr("Tinker's Construct"), "addons-tinkers-construct" },
        { 419, tr("Magic"), "magic" },
        { 412, tr("Technology"), "technology" },
        { 414, tr("Player Transport"), "technology-player-transport" },
        { 4486, tr("Lucky Blocks"), "[4486]lucky-blocks" },
        { 432, tr("Buildcraft"), "addons-buildcraft" },
        { 418, tr("Genetics"), "technology-genetics" },
        { 4671, tr("Twitch Integration"), "twitch-integration" },
        { 408, tr("Ores and Resources"), "world-ores-resources" },
        { 4773, tr("CraftTweaker"), "crafttweaker" },
        { 430, tr("Thaumcraft"), "addons-thaumcraft" },
        { 422, tr("Adventure and RPG"), "adventure-rpg" },
        { 413, tr("Processing"), "technology-processing" },
        { 433, tr("Forestry"), "addons-forestry" },
        { 416, tr("Farming"), "technology-farming" },
        { 417, tr("Energy"), "technology-energy" },
        { 415, tr("Energy, Fluid, and Item Transport"), "technology-item-fluid-energy-transport" },
        { 425, tr("Miscellaneous"), "mc-miscellaneous" },
        { 4545, tr("Applied Energistics 2"), "applied-energistics-2" },
        { 421, tr("API and Library"), "library-api" },
        { 424, tr("Cosmetic"), "cosmetic" },
        { 411, tr("Mobs"), "world-mobs" },
        { 406, tr("World Gen"), "world-gen" },
        { 435, tr("Server Utility"), "server-utility" },
        { 407, tr("Biomes"), "world-biomes" },
        { 427, tr("Thermal Expansion"), "addons-thermalexpansion" },
        { 410, tr("Dimensions"), "world-dimensions" },
        { 436, tr("Food"), "mc-food" },
        { 4558, tr("Redstone"), "redstone" },
        { 4843, tr("Automation"), "technology-automation" },
        { 4906, tr("MCreator"), "mc-creator" },
        { 4780, tr("Fabric"), "[4780]fabric"}
    };
    return categories;
}
