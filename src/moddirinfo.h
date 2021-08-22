#ifndef MODDIRINFO_H
#define MODDIRINFO_H

#include <QDir>

#include "gameversion.h"
#include "modloadertype.h"

class ModDirInfo
{
public:
    ModDirInfo(const QDir &dir, const GameVersion &version, ModLoaderType::Type type);

    bool exists() const;

    const QDir &getModDir() const;
    const GameVersion &getGameVersion() const;

    ModLoaderType::Type getLoaderType() const;

private:
    QDir modDir;
    GameVersion gameVersion;
    ModLoaderType::Type loaderType;
};

#endif // MODDIRINFO_H
