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
        return "Fabric";
    case Type::Forge:
        return "Forge";
    case Type::Rift:
        return "Rift";
    default:
        return "";
    }
}
