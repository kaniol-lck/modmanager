#ifndef LOCALMODPATHINFO_H
#define LOCALMODPATHINFO_H

#include <QVariant>

#include "gameversion.h"
#include "modloadertype.h"

class LocalModPathInfo
{
public:
    LocalModPathInfo() = default;
    LocalModPathInfo(const QString &dir, const GameVersion &version, ModLoaderType::Type type);
    LocalModPathInfo(const QString &name, const QString &dir, const GameVersion &version, ModLoaderType::Type type);

    bool operator==(const LocalModPathInfo &other) const;
    bool operator!=(const LocalModPathInfo &other) const;

    static LocalModPathInfo fromVariant(const QVariant &variant);
    QVariant toVariant() const;

    QString displayName() const;
    QIcon icon() const;

    bool exists() const;

    void setGameVersion(const GameVersion &newGameVersion);
    const GameVersion &gameVersion() const;

    void setLoaderType(ModLoaderType::Type newLoaderType);
    ModLoaderType::Type loaderType() const;

    const QString &name() const;
    void setName(const QString &newName);

    bool isAutoName() const;
    void setIsAutoName(bool newIsAutoName);
    QString autoName() const;

    const QString &path() const;
    void setPath(const QString &newPath);

    static LocalModPathInfo deduceFromPath(const QString &path);

private:
    QString name_;
    QString path_;
    GameVersion gameVersion_ = GameVersion::Any;
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;
    bool isAutoName_;
};

#endif // LOCALMODPATHINFO_H
