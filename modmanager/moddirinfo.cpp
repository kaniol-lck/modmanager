#include "moddirinfo.h"

ModDirInfo::ModDirInfo(const QDir &dir, const GameVersion &version, const QString &type) :
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

const QString &ModDirInfo::getLoaderType() const
{
    return loaderType;
}
