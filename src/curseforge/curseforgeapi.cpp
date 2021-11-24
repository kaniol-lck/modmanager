#include "curseforgeapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "util/mmlogger.h"
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

QMetaObject::Connection CurseforgeAPI::searchMods(const GameVersion &version, int index, const QString &searchFilter, int category, int sort, std::function<void (QList<CurseforgeModInfo>)> callback)
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
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
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

QMetaObject::Connection CurseforgeAPI::getIdByFingerprint(const QString &fingerprint, std::function<void (int, CurseforgeFileInfo, QList<CurseforgeFileInfo>)> callback, std::function<void ()> noMatch)
{
    QUrl url = PREFIX + "/api/v2/fingerprint";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    MMLogger::network(this) << url << fingerprint;
    auto reply = accessManager_.post(request, QString("[ %1 ]").arg(fingerprint).toUtf8());
    return connect(reply, &QNetworkReply::finished, this, [=]{
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

QMetaObject::Connection CurseforgeAPI::getDescription(int id, std::function<void (QString)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/description";

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        auto description = reply->readAll();
        callback(description);
        reply->deleteLater();
    });
}

QMetaObject::Connection CurseforgeAPI::getFileInfo(int id, int FileID, std::function<void (CurseforgeFileInfo)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/file/" + QString::number(FileID);

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
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

QMetaObject::Connection CurseforgeAPI::getFiles(int id, std::function<void (QList<CurseforgeFileInfo>)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id) + "/files";

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
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
        auto resultList = jsonDocument.toVariant().toList();

        QList<CurseforgeFileInfo> fileInfoList;
        for(const auto &result : qAsConst(resultList))
            fileInfoList << CurseforgeFileInfo::fromVariant(result);

        callback(fileInfoList);
        reply->deleteLater();
    });
}

QMetaObject::Connection CurseforgeAPI::getInfo(int id, std::function<void (CurseforgeModInfo)> callback)
{
    QUrl url = PREFIX + "/api/v2/addon/" + QString::number(id);

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
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

QMetaObject::Connection CurseforgeAPI::getMinecraftVersionList(std::function<void (QList<GameVersion>)> callback)
{
    QUrl url = PREFIX + "/api/v2/minecraft/version";

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
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

const QList<std::tuple<int, QString, QString, int> > &CurseforgeAPI::getCategories()
{
    static const QList<std::tuple<int, QString, QString, int>> categories{
        { 423, tr("Map and Information"), "map-information", 6 },
        { 426, tr("Addons"), "mc-addons", 6 },
        { 434, tr("Armor, Tools, and Weapons"), "armor-weapons-tools", 6 },
        { 409, tr("Structures"), "world-structures", 406 },
        { 4485, tr("Blood Magic"), "blood-magic", 426 },
        { 429, tr("Industrial Craft"), "addons-industrialcraft", 426 },
        { 4671, tr("Twitch Integration"), "twitch-integration", 6 },
        { 428, tr("Tinker's Construct"), "addons-tinkers-construct", 426 },
        { 419, tr("Magic"), "magic", 6 },
        { 420, tr("Storage"), "storage", 6 },
        { 430, tr("Thaumcraft"), "addons-thaumcraft", 426 },
        { 4486, tr("Lucky Blocks"), "[4486]lucky-blocks", 426 },
        { 433, tr("Forestry"), "addons-forestry", 426 },
        { 412, tr("Technology"), "technology", 6 },
        { 414, tr("Player Transport"), "technology-player-transport", 412 },
        { 415, tr("Energy, Fluid, and Item Transport"), "technology-item-fluid-energy-transport", 412 },
        { 432, tr("Buildcraft"), "addons-buildcraft", 426 },
        { 418, tr("Genetics"), "technology-genetics", 412 },
        { 408, tr("Ores and Resources"), "world-ores-resources", 406 },
        { 413, tr("Processing"), "technology-processing", 412 },
        { 4773, tr("CraftTweaker"), "crafttweaker", 426 },
        { 416, tr("Farming"), "technology-farming", 412 },
        { 422, tr("Adventure and RPG"), "adventure-rpg", 6 },
        { 424, tr("Cosmetic"), "cosmetic", 6 },
        { 4545, tr("Applied Energistics 2"), "applied-energistics-2", 426 },
        { 421, tr("API and Library"), "library-api", 6 },
        { 417, tr("Energy"), "technology-energy", 412 },
        { 425, tr("Miscellaneous"), "mc-miscellaneous", 6 },
        { 435, tr("Server Utility"), "server-utility", 6 },
        { 427, tr("Thermal Expansion"), "addons-thermalexpansion", 426 },
        { 410, tr("Dimensions"), "world-dimensions", 406 },
        { 411, tr("Mobs"), "world-mobs", 406 },
        { 407, tr("Biomes"), "world-biomes", 406 },
        { 4558, tr("Redstone"), "redstone", 6 },
        { 406, tr("World Gen"), "world-gen", 6 },
        { 436, tr("Food"), "mc-food", 6 },
        { 4843, tr("Automation"), "technology-automation", 412 },
        { 5192, tr("FancyMenu"), "fancymenu", 6 },
        { 4906, tr("MCreator"), "mc-creator", 6 },
        { 5189, tr("Utility & QOL"), "[5189]utility-qol", 6 },
        { 4780, tr("Fabric"), "[4780]fabric", 6 },
        { 5129, tr("Vanilla+"), "[5129]vanilla", 6 },
        { 5190, tr("QoL"), "[5190]qol", 6 },
        { 5191, tr("Utility"), "utility", 6 }
    };
    return categories;
}
