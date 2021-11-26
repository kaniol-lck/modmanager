#include "modwebsitetype.h"

QString ModWebsite::toString(ModWebsiteType type)
{
    switch (type) {
    case ModWebsiteType::None:
        return QObject::tr("None");
    case ModWebsiteType::Curseforge:
        return "Curseforge";
    case ModWebsiteType::Modrinth:
        return "Modrinth";
    }
}

QIcon ModWebsite::icon(ModWebsiteType type)
{
    switch (type) {
    case ModWebsiteType::None:
        return QIcon();
    case ModWebsiteType::Curseforge:
        return QIcon(":/image/curseforge.svg");
    case ModWebsiteType::Modrinth:
        return QIcon(":/image/modrinth.svg");
    }
}
