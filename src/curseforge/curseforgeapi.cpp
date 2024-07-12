#include "curseforgeapi.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "util/mmlogger.h"
#include "config.hpp"

const QString CurseforgeAPI::PREFIX = "https://api.curseforge.com";
const QByteArray CurseforgeAPI::XAPIKEY = "$2a$10$o8pygPrhvKBHuuh5imL2W.LCNFhB15zBYAExXx/TqTx/Zp5px2lxu";

CurseforgeAPI::CurseforgeAPI(QObject *parent) :
    QObject(parent)
{
    accessManager_.setTransferTimeout(Config().getNetworkRequestTimeout());
    diskCache_.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    accessManager_.setCache(&diskCache_);
}

CurseforgeAPI *CurseforgeAPI::api()
{
    static CurseforgeAPI api;
    return &api;
}

Reply<QList<CurseforgeModInfo> > CurseforgeAPI::searchMods(int sectionId, const GameVersion &version, const ModLoaderType::Type &loaderType, int index, const QString &searchFilter, int category, int sort, bool sortOrderAsc)
{
    QUrl url = PREFIX + "/v1/mods/search";

    //url query
    QUrlQuery urlQuery;

    //?
    urlQuery.addQueryItem("categoryId", QString::number(category));
    //minecraft
    urlQuery.addQueryItem("gameId", "432");
    //game version
    if(version != GameVersion::Any)
        urlQuery.addQueryItem("gameVersion", version);
    //loader type
    if(loaderType != ModLoaderType::Any)
        urlQuery.addQueryItem("modLoaderType", QString::number(ModLoaderType::curseforge.indexOf(loaderType)));
    //index
    urlQuery.addQueryItem("index", QString::number(index));
    //search page size
    urlQuery.addQueryItem("pageSize", QString::number(Config().getSearchResultCount()));
    //search by name
    urlQuery.addQueryItem("searchFilter", searchFilter);
    //mod
    urlQuery.addQueryItem("classId", QString::number(sectionId));
    //sort, 0 for no sort spec
    urlQuery.addQueryItem("sortField", QString::number(sort + 1));
    urlQuery.addQueryItem("sortOrder", sortOrderAsc? "asc": "desc");

    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    request.setRawHeader("x-api-key",XAPIKEY);
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;

        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);

        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<CurseforgeModInfo>{};
        }
        auto resultList = value(jsonDocument.toVariant(),"data").toList();
        QList<CurseforgeModInfo> modInfoList;

        for(const auto &result : qAsConst(resultList))
            modInfoList << CurseforgeModInfo::fromVariant(result);

        return modInfoList;
    } };
}

Reply<int, CurseforgeFileInfo, QList<CurseforgeFileInfo> > CurseforgeAPI::getIdByFingerprint(const QString &fingerprint)
{
    QUrl url = PREFIX + "/v1/fingerprints";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key",XAPIKEY);
    MMLogger::network(this) << url << fingerprint;
    auto reply = accessManager_.post(request, QString("{\"fingerprints\":[ %1 ]}").arg(fingerprint).toUtf8());
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return std::make_tuple(0, CurseforgeFileInfo(), QList<CurseforgeFileInfo>{});
        }
        auto exactMatchList = value(value(jsonDocument.toVariant(), "data"),"exactMatches").toList();
        if(exactMatchList.isEmpty())
            return std::make_tuple(0, CurseforgeFileInfo(), QList<CurseforgeFileInfo>{});
        else {
            int id = value(exactMatchList.at(0), "id").toInt();
            auto file = CurseforgeFileInfo::fromVariant(value(exactMatchList.at(0), "file"));
            auto latestFileList = value(exactMatchList.at(0), "latestFiles").toList();

            QList<CurseforgeFileInfo> fileList;
            for (const auto &variant : latestFileList)
                fileList << CurseforgeFileInfo::fromVariant(variant);

            return std::make_tuple(id, file, fileList);
        }
    } };
}

Reply<QString> CurseforgeAPI::getDescription(int id)
{
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id) + "/description";

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    request.setRawHeader("x-api-key",XAPIKEY);
    auto reply = accessManager_.get(request);
    return { reply, [=]{
                //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QString();
        }
        return value(jsonDocument.toVariant(),"data").toString();
    } };
}

Reply<CurseforgeFileInfo> CurseforgeAPI::getFileInfo(int id, int FileID)
{
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id) + "/files/" + QString::number(FileID);

    QNetworkRequest request(url);
    MMLogger::network(this) << url;
    request.setRawHeader("x-api-key", XAPIKEY);
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return CurseforgeFileInfo();
        }
        auto result = value(jsonDocument.toVariant(),"data");
        return CurseforgeFileInfo::fromVariant(result);
    } };
}

Reply<QList<CurseforgeFileInfo>, int> CurseforgeAPI::getModFiles(int id, int index, GameVersion version, ModLoaderType::Type loaderType)
{
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id) + "/files";
    //url query
    QUrlQuery urlQuery;
    //version
    if(version != GameVersion::Any)
        urlQuery.addQueryItem("gameVersion", version.toString());
    //loader tye
    if(loaderType != ModLoaderType::Any)
        urlQuery.addQueryItem("modLoaderType", QString::number(ModLoaderType::curseforge.indexOf(loaderType)));
    //index
    urlQuery.addQueryItem("index", QString::number(index));
    //page size
    urlQuery.addQueryItem("pageSize", QString::number(Config().getSearchResultCount()));
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setRawHeader("x-api-key",XAPIKEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return std::make_tuple(QList<CurseforgeFileInfo>{}, 0);
        }
        auto resultList = value(jsonDocument.toVariant(), "data").toList();
        auto totalCount = value(jsonDocument.toVariant(), "pagination", "totalCount").toInt();

        QList<CurseforgeFileInfo> fileInfoList;
        for(const auto &result : qAsConst(resultList))
            fileInfoList << CurseforgeFileInfo::fromVariant(result);

        return std::make_tuple(fileInfoList, totalCount);
        } };
}

Reply<QList<CurseforgeFileInfo> > CurseforgeAPI::getFiles(QList<int> fileIds)
{
    QUrl url = PREFIX + "/v1/mods/files";

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key",XAPIKEY);
    MMLogger::network(this) << url;
    QJsonArray list;
    for(auto &&fileId : fileIds)
        list << fileId;
    QJsonObject data{
        { "fileIds", list }
    };
    auto reply = accessManager_.post(request, QJsonDocument(data).toJson(QJsonDocument::Compact));
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return std::make_tuple(QList<CurseforgeFileInfo>{});
        }
        auto resultList = value(jsonDocument.toVariant(), "data").toList();

        QList<CurseforgeFileInfo> fileInfoList;
        for(const auto &result : qAsConst(resultList))
            fileInfoList << CurseforgeFileInfo::fromVariant(result);

        return std::make_tuple(fileInfoList);
        } };
}

Reply<CurseforgeModInfo> CurseforgeAPI::getInfo(int id)
{
    QUrl url = PREFIX + "/v1/mods/" + QString::number(id);

    QNetworkRequest request(url);
    request.setRawHeader("x-api-key",XAPIKEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return CurseforgeModInfo();
        }
        auto result = value(jsonDocument.toVariant(),"data");
        return CurseforgeModInfo::fromVariant(result);
    } };
}

Reply<QList<GameVersion> > CurseforgeAPI::getMinecraftVersionList()
{
    QUrl url = PREFIX + "/v1/minecraft/version";

    QNetworkRequest request(url);
    request.setRawHeader("x-api-key",XAPIKEY);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return Reply<QList<GameVersion> >(reply, [=]{
        //parse json
        QJsonParseError error;
        auto jsonVariant = value(QJsonDocument::fromJson(reply->readAll(), &error).toVariant(),"data");
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<GameVersion>{};
        }
        auto list = jsonVariant.toList();
        QList<GameVersion> versionList;
        for(const auto &entry : qAsConst(list))
            versionList << value(entry, "versionString").toString();

        return versionList;
    });
}

Reply<QList<CurseforgeCategoryInfo> > CurseforgeAPI::getSectionCategories(int sectionId)
{
    QUrl url = PREFIX + "/v1/categories?gameId=432&classId=" + QString::number(sectionId);

    QNetworkRequest request(url);
    request.setRawHeader("x-api-key",XAPIKEY);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    MMLogger::network(this) << url;
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        auto jsonVariant = value(QJsonDocument::fromJson(reply->readAll(), &error).toVariant(),"data");
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<CurseforgeCategoryInfo>{};
        }
        QFile cachedFile(cachedCategoriesFilePath(sectionId));
        if(cachedFile.open(QIODevice::WriteOnly)){
            cachedFile.write(jsonVariant.toJsonDocument().toJson());
            cachedFile.close();
        }
        auto list = jsonVariant.toList();
        QList<CurseforgeCategoryInfo> categoryList;
        for(const auto &entry : qAsConst(list)){
            auto category = CurseforgeCategoryInfo::fromVariant(entry);
            if(category.url().isEmpty() || category.url() == QUrl("https://www.curseforge.com"))
                continue;
            categoryList << category;
        }
        return categoryList;
        } };
}

QList<CurseforgeCategoryInfo> CurseforgeAPI::cachedSectionCategories(int sectionId)
{
    QByteArray bytes;
    QFile cachedFile(cachedCategoriesFilePath(sectionId));
    if(!cachedFile.open(QIODevice::ReadOnly)) return {};
    bytes = cachedFile.readAll();
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return QList<CurseforgeCategoryInfo>{};
    }
    auto list = jsonDocument.toVariant().toList();
    QList<CurseforgeCategoryInfo> categoryList;
    for(const auto &entry : qAsConst(list)){
        auto category = CurseforgeCategoryInfo::fromVariant(entry);
        if(category.url().isEmpty() || category.url() == QUrl("https://www.curseforge.com"))
            continue;
        categoryList << category;
    }
    return categoryList;
}

QString CurseforgeAPI::cachedCategoriesFilePath(int sectionId)
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
            .absoluteFilePath("curseforge/section-%1-categories.json").arg(sectionId);
}
