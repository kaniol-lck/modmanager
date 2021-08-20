#include "modloadertype.h"

#include <QObject>

ModLoaderType::Type ModLoaderType::fromString(const QString &str)
{
    if(str.toLower() == "fabric")
        return Type::Fabric;
    else if(str.toLower() == "forge")
        return Type::Forge;
    else if(str.toLower() == "rift")
        return Type::Rift;
    else
        return Type::Any;
}

QString ModLoaderType::toString(Type loaderType)
{
    switch (loaderType) {
    case Type::Any:
        return QObject::tr("Any");
    case Type::Fabric:
        return QObject::tr("Fabric");
    case Type::Forge:
        return QObject::tr("Forge");
    case Type::Rift:
        return QObject::tr("Rift");
    default:
        return "";
    }
}

ModLoaderType::Type ModLoaderType::fromIndex(int index)
{
    return static_cast<ModLoaderType::Type>(index);
}
