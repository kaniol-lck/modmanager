#include "moddirinfo.h"

#include "util/tutil.hpp"

ModDirInfo::ModDirInfo(const QDir &dir, const GameVersion &version, ModLoaderType::Type type) :
    modDir(dir),
    gameVersion(version),
    loaderType(type)
{

}

bool ModDirInfo::operator==(const ModDirInfo &other) const
{
    if(modDir == other.modDir && gameVersion == other.gameVersion && loaderType == other.loaderType)
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
    info.modDir.setPath(value(variant, "dir").toString());
    info.gameVersion = value(variant, "gameVersion").toString();
    info.loaderType = ModLoaderType::fromString(value(variant, "loaderType").toString());

    return info;
}

QVariant ModDirInfo::toVariant() const
{
    QMap<QString, QVariant> map;
    map["dir"] = modDir.path();
    map["gameVersion"] = QString(gameVersion);
    map["loaderType"] = ModLoaderType::toString(loaderType);

    return QVariant::fromValue(map);
}

QString ModDirInfo::showText() const
{
    return gameVersion + " - " + ModLoaderType::toString(loaderType);
}

bool ModDirInfo::exists() const
{
    return modDir.exists();
}

const QDir &ModDirInfo::getModDir() const
{
    return modDir;
}

const GameVersion &ModDirInfo::getGameVersion() const
{
    return gameVersion;
}

ModLoaderType::Type ModDirInfo::getLoaderType() const
{
    return loaderType;
}
