#ifndef MODLOADERTYPE_H
#define MODLOADERTYPE_H

#include <QString>
#include <QList>
#include <QIcon>

namespace ModLoaderType
{
enum Type{
    Any,
    Cauldron, //is it alive?
    Fabric,
    Forge,
    LiteLoader,
    NeoForge,
    Quilt,
    Rift
};

Type fromString(const QString &str);
QString toString(Type loaderType);

QIcon icon(Type type);

const QList<Type> curseforge{ Any, Forge, Cauldron, LiteLoader, Fabric, Quilt, NeoForge };
const QList<Type> modrinth{ Any, Fabric, Forge, LiteLoader, NeoForge, Quilt, Rift };
const QList<Type> optifine{ Any, Fabric, Forge };
const QList<Type> replay{ Any, Fabric, Forge };
const QList<Type> local{ Any, Fabric, Forge };

}
#endif // MODLOADERTYPE_H
