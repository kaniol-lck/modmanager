#ifndef MODDIRINFO_H
#define MODDIRINFO_H

#include <QDir>

#include "gameversion.h"

class ModDirInfo
{
public:
    ModDirInfo(const QDir &dir, const GameVersion &version, const QString &type);

    bool exists() const;

    const QDir &getModDir() const;
    const GameVersion &getGameVersion() const;

    const QString &getLoaderType() const;

private:
    QDir modDir;
    GameVersion gameVersion;
    QString loaderType;
};

#endif // MODDIRINFO_H
