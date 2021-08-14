#include "moddirinfo.h"

ModDirInfo::ModDirInfo(QDir dir, QString version) :
    modDir(dir),
    gameVersion(version)
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

const QString &ModDirInfo::getGameVersion() const
{
    return gameVersion;
}
