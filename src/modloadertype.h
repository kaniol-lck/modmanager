#ifndef MODLOADERTYPE_H
#define MODLOADERTYPE_H

#include <QString>
#include <QList>

namespace ModLoaderType
{
enum Type{
    Any,
    Fabric,
    Forge,
    Rift
};

Type fromString(const QString &str);
QString toString(Type loaderType);

const QList<Type> local{ Fabric };
const QList<Type> curseforge{ Any, Fabric, Forge/*, Rift*/ };
const QList<Type> modrinth{ Any, Fabric, Forge };

};

#endif // MODLOADERTYPE_H
