#include "modrinthfileinfo.h"

#include <QJsonObject>
#include <QJsonArray>

#include "util/tutil.hpp"

ModrinthFileInfo ModrinthFileInfo::fromVariant(const QVariant &variant)
{
    ModrinthFileInfo fileInfo;

    fileInfo.id_ = value(variant, "id").toString();
    fileInfo.modId_ = value(variant, "mod_id").toString();
    fileInfo.displayName_ = value(variant, "name").toString();
    fileInfo.releaseType_ = value(variant, "version_type").toString();
    fileInfo.fileDate_ = value(variant, "date_published").toDateTime();
    fileInfo.isFeatured_ = value(variant, "featured").toBool();

    auto files = value(variant, "files").toList();

    if(!files.isEmpty()){
        fileInfo.fileName_ = value(files.at(0), "filename").toString();
        fileInfo.url_ = value(files.at(0), "url").toUrl();
        fileInfo.isPrimary_ = value(files.at(0), "primary").toBool();
    }

    auto versionList = value(variant, "game_versions").toStringList();
    for(const auto &version : versionList)
        fileInfo.gameVersions_ << GameVersion(version);

    auto loaderList = value(variant, "loaders").toStringList();
    for(const auto &loader : loaderList)
        fileInfo.loaderTypes_ << ModLoaderType::fromString(loader);

    return fileInfo;
}

QJsonObject ModrinthFileInfo::toJsonObject() const
{
    QJsonObject object{
        {"id", id_},
        {"mod_id", modId_},
        {"name", displayName_},
        {"version_type", releaseType_},
        {"date_published", fileDate_.toString(Qt::DateFormat::ISODate)},
        {"featured", isFeatured_}
    };
    QJsonArray fileArray;
    fileArray << QJsonObject{
        {"filename", fileName_},
        {"url", url_.toString()},
        {"primary", isPrimary_}
    };
    object.insert("files", fileArray);
    return object;
}

const ModrinthFileInfo::IdType &ModrinthFileInfo::id() const
{
    return id_;
}

const QString &ModrinthFileInfo::modId() const
{
    return modId_;
}

const QString &ModrinthFileInfo::displayName() const
{
    return displayName_;
}

const QString &ModrinthFileInfo::fileName() const
{
    return fileName_;
}

const QUrl &ModrinthFileInfo::url() const
{
    return url_;
}

qint64 ModrinthFileInfo::size() const
{
    return size_;
}

const QList<GameVersion> &ModrinthFileInfo::gameVersions() const
{
    return gameVersions_;
}

const QList<ModLoaderType::Type> &ModrinthFileInfo::loaderTypes() const
{
    return loaderTypes_;
}

const QString &ModrinthFileInfo::releaseType() const
{
    return releaseType_;
}

const QDateTime &ModrinthFileInfo::fileDate() const
{
    return fileDate_;
}

bool ModrinthFileInfo::isFeatured() const
{
    return isFeatured_;
}

bool ModrinthFileInfo::isPrimary() const
{
    return isPrimary_;
}

bool ModrinthFileInfo::operator==(const ModrinthFileInfo &info) const
{
    return id_ == info.id_;
}
