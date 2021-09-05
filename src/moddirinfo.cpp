#include "moddirinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

ModDirInfo::ModDirInfo(const QDir &dir, const GameVersion &version, ModLoaderType::Type type) :
    modDir_(dir),
    gameVersion_(version),
    loaderType_(type)
{
    name_ = showText();
}

ModDirInfo::ModDirInfo(const QString &name, const QDir &dir, const GameVersion &version, ModLoaderType::Type type) :
    name_(name),
    modDir_(dir),
    gameVersion_(version),
    loaderType_(type)
{

}

bool ModDirInfo::operator==(const ModDirInfo &other) const
{
    //no name
    if(modDir_ == other.modDir_ && gameVersion_ == other.gameVersion_ && loaderType_ == other.loaderType_)
        return true;
    else
        return false;
}

bool ModDirInfo::operator!=(const ModDirInfo &other) const
{
    return !operator==(other);
}

ModDirInfo ModDirInfo::fromVariant(const QVariant &variant)
{
    ModDirInfo info;
    info.name_ = value(variant, "name").toString();
    info.modDir_.setPath(value(variant, "dir").toString());
    info.gameVersion_ = value(variant, "gameVersion").toString();
    info.loaderType_ = ModLoaderType::fromString(value(variant, "loaderType").toString());

    return info;
}

QVariant ModDirInfo::toVariant() const
{
    QMap<QString, QVariant> map;
    map["name"] = name_;
    map["dir"] = modDir_.path();
    map["gameVersion"] = QString(gameVersion_);
    map["loaderType"] = ModLoaderType::toString(loaderType_);

    return QVariant::fromValue(map);
}

QString ModDirInfo::showText() const
{
    return isAutoName()? autoName() : name_;
}

bool ModDirInfo::exists() const
{
    return modDir_.exists();
}

const QDir &ModDirInfo::modDir() const
{
    return modDir_;
}

const GameVersion &ModDirInfo::gameVersion() const
{
    return gameVersion_;
}

ModLoaderType::Type ModDirInfo::loaderType() const
{
    return loaderType_;
}

const QString &ModDirInfo::name() const
{
    return name_;
}

void ModDirInfo::setName(const QString &newName)
{
    name_ = newName;
}

bool ModDirInfo::isAutoName() const
{
    return name_ == autoName();
}

QString ModDirInfo::autoName() const
{
    return gameVersion_ + " - " + ModLoaderType::toString(loaderType_);
}

void ModDirInfo::setModDir(const QDir &newModDir)
{
    modDir_ = newModDir;
}

void ModDirInfo::setGameVersion(const GameVersion &newGameVersion)
{
    gameVersion_ = newGameVersion;
}

void ModDirInfo::setLoaderType(ModLoaderType::Type newLoaderType)
{
    loaderType_ = newLoaderType;
}
