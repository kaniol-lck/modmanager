#ifndef MODLOADERTYPE_H
#define MODLOADERTYPE_H

#include <QString>

namespace ModLoaderType
{
enum Type{
    Any,
    Fabric,
    Forge,
    Rift
};

Type fromIndex(int index);
Type fromString(const QString &str);
QString toString(Type loaderType);

};

#endif // MODLOADERTYPE_H
