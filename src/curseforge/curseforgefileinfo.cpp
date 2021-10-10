#include "curseforgefileinfo.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "util/tutil.hpp"

CurseforgeFileInfo CurseforgeFileInfo::fromVariant(const QVariant &variant)
{
    CurseforgeFileInfo fileInfo;

    fileInfo.id_ = value(variant, "id").toInt();
    fileInfo.displayName_ = value(variant, "displayName").toString();
    fileInfo.fileName_ = value(variant, "fileName").toString();
    fileInfo.url_ = value(variant, "downloadUrl").toUrl();
    fileInfo.size_ = value(variant, "fileLength").toInt();
    fileInfo.releaseType_ = value(variant, "releaseType").toInt();
    fileInfo.fileDate_ = value(variant, "fileDate").toDateTime();

    //I don't know why curseforge put game verison and modloader together
    for(auto &&version : value(variant, "gameVersion").toStringList()){
        if(auto loaderType = ModLoaderType::fromString(version); loaderType != ModLoaderType::Any)
            fileInfo.loaderTypes_ << loaderType;
        else {
            if(auto gameVersion = GameVersion::deduceFromString(version); gameVersion != GameVersion::Any)
                fileInfo.gameVersions_ << gameVersion;
            else
                fileInfo.gameVersions_ << version;
        }
    }

    //fields: addonId id type
    for(auto &&dependency : value(variant, "dependencies").toList())
        fileInfo.dependencies_ << value(dependency, "fileId").toInt();

    return fileInfo;
}

QJsonObject CurseforgeFileInfo::toJsonObject() const
{
    QJsonObject object{
        {"id", id_},
        {"displayName", displayName_},
        {"fileName", fileName_},
        {"downloadUrl", url_.toString()},
        {"fileLength", size_},
        {"releaseType", releaseType_},
        {"fileDate", fileDate_.toString(Qt::DateFormat::ISODate)}
    };
    QJsonArray versionArray;
    for(const auto &type : loaderTypes_)
        versionArray << ModLoaderType::toString(type);
    for(const auto &version : gameVersions_)
        versionArray << QString(version);
    object.insert("gameVersion", versionArray);
    return object;
}

CurseforgeFileInfo::IdType CurseforgeFileInfo::id() const
{
    return id_;
}

const QString &CurseforgeFileInfo::displayName() const
{
    return displayName_;
}

const QString &CurseforgeFileInfo::fileName() const
{
    return fileName_;
}

const QUrl &CurseforgeFileInfo::url() const
{
    return url_;
}

qint64 CurseforgeFileInfo::size() const
{
    return size_;
}

const QList<GameVersion> &CurseforgeFileInfo::gameVersions() const
{
    return gameVersions_;
}

const QList<ModLoaderType::Type> &CurseforgeFileInfo::loaderTypes() const
{
    return loaderTypes_;
}

int CurseforgeFileInfo::releaseType() const
{
    return releaseType_;
}

const QDateTime &CurseforgeFileInfo::fileDate() const
{
    return fileDate_;
}
