#ifndef MODWEBSITETYPE_H
#define MODWEBSITETYPE_H

#include <QString>
#include <QList>
#include <QIcon>

enum ModWebsiteType{
    None,
    Curseforge,
    Modrinth
};

namespace ModWebsite
{
QString toString(ModWebsiteType type);
QIcon icon(ModWebsiteType type);
}

//type traits
//some common classes for curseforge / modrinth
template<ModWebsiteType type>
struct CommonClass{
};

class CurseforgeMod;
class CurseforgeFileInfo;
template<>
struct CommonClass<Curseforge>
{
    using Mod = CurseforgeMod;
    using FileInfo = CurseforgeFileInfo;
    using Id = int;
    static bool isEmptyId(Id id)
    {
        return id;
    }
};

class ModrinthMod;
class ModrinthFileInfo;
template<>
struct CommonClass<Modrinth>
{
    using Mod = ModrinthMod;
    using FileInfo = ModrinthFileInfo;
    using Id = QString;
    static bool isEmptyId(Id id)
    {
        return id.isEmpty();
    }
};
#endif // MODWEBSITETYPE_H
