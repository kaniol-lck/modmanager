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
    ModDirInfo(const QString &name, const QDir &dir, const GameVersion &version, ModLoaderType::Type type);

    bool operator==(const ModDirInfo &other) const;
    bool operator!=(const ModDirInfo &other) const;

    static ModDirInfo fromVariant(const QVariant &variant);
    QVariant toVariant() const;

    QString showText() const;

    bool exists() const;

    void setModDir(const QDir &newModDir);
    const QDir &modDir() const;

    void setGameVersion(const GameVersion &newGameVersion);
    const GameVersion &gameVersion() const;

    void setLoaderType(ModLoaderType::Type newLoaderType);
    ModLoaderType::Type loaderType() const;

    const QString &name() const;
    void setName(const QString &newName);

    bool isAutoName() const;
    QString autoName() const;

private:
    QString name_;
    QDir modDir_;
    GameVersion gameVersion_;
    ModLoaderType::Type loaderType_;
};

#endif // MODDIRINFO_H
