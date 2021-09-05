#ifndef MODDIRINFO_H
#define MODDIRINFO_H

#include <QDir>

#include "gameversion.h"
#include "modloadertype.h"

class ModDirInfo
{
public:
    ModDirInfo() = default;
    ModDirInfo(const QDir &dir, const GameVersion &version, ModLoaderType::Type type);

    bool operator==(const ModDirInfo &other) const;
    bool operator!=(const ModDirInfo &other) const;

    static ModDirInfo fromVariant(const QVariant &variant);
    QVariant toVariant() const;

    QString showText() const;

    bool exists() const;

    const QDir &modDir() const;
    const GameVersion &gameVersion() const;

    ModLoaderType::Type loaderType() const;

private:
    QDir modDir_;
    GameVersion gameVersion_;
    ModLoaderType::Type loaderType_;
};

#endif // MODDIRINFO_H
