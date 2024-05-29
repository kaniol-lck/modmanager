#include "modloadertype.h"

#include <QObject>

ModLoaderType::Type ModLoaderType::fromString(const QString &str)
{
    if(str.toLower() == "fabric")
        return Type::Fabric;
    else if(str.toLower() == "forge")
        return Type::Forge;
    else if(str.toLower() == "liteloader")
        return Type::LiteLoader;
    else if(str.toLower() == "neoforge")
        return Type::Neoforge;
    else if(str.toLower() == "quilt")
        return Type::Quilt;
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
    case Type::LiteLoader:
        return "LiteLoader";
    case Type::Neoforge:
        return "NeoForge";
    case Type::Quilt:
        return "Quilt";
    case Type::Rift:
        return "Rift";
    default:
        return "";
    }
}

QIcon ModLoaderType::icon(Type type)
{
    if(type == Fabric)
        return QIcon(":/image/fabric.png");
    if(type == Forge)
        return QIcon(":/image/forge.svg");
    if(type == LiteLoader)
        return QIcon(":/image/liteloader.png");
    if(type == Neoforge)
        return QIcon(":/image/neoforge.png");
    if(type == Quilt)
        return QIcon(":/image/quilt.svg");
    if(type == Rift)
        return QIcon(":/image/rift.png");
    return QIcon();
}
