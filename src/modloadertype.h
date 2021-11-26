#ifndef MODLOADERTYPE_H
#define MODLOADERTYPE_H

#include <QString>
#include <QList>
#include <QIcon>

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

QIcon icon(Type type);

const QList<Type> curseforge{ Any, Fabric, Forge/*, Rift*/ };
const QList<Type> modrinth{ Any, Fabric, Forge };
const QList<Type> replay{ Any, Fabric, Forge };
const QList<Type> local{ Any, Fabric, Forge };

}
#endif // MODLOADERTYPE_H
