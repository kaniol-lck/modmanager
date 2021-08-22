#include "moddirinfo.h"

ModDirInfo::ModDirInfo(const QDir &dir, const GameVersion &version, ModLoaderType::Type type) :
    modDir(dir),
    gameVersion(version),
    loaderType(type)
{

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
