#ifndef MODWEBSITETYPE_H
#define MODWEBSITETYPE_H

#include <QString>
#include <QList>
#include <QIcon>

enum class ModWebsiteType{
    None,
    Curseforge,
    Modrinth
};

namespace ModWebsite
{
QString toString(ModWebsiteType type);
QIcon icon(ModWebsiteType type);
}
#endif // MODWEBSITETYPE_H
