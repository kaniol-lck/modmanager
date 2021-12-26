#include "curseforgecategoryinfo.h"

#include "util/tutil.hpp"

#include <QDir>
#include <QStandardPaths>
#include <QDebug>

const QString &CurseforgeCategoryInfo::cachePath()
{
    static const QString path =
            QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
            .absoluteFilePath("curseforge");
    return path;
}

CurseforgeCategoryInfo CurseforgeCategoryInfo::fromVariant(const QVariant &variant)
{
    CurseforgeCategoryInfo info;
    if(variant.toMap().contains("id"))
        info.id_ = value(variant, "id").toInt();
    else
        info.id_ = value(variant, "categoryId").toInt();
    info.name_ = value(variant, "name").toString();
    info.slug_ = value(variant, "slug").toString();
    info.avatarUrl_ = value(variant, "avatarUrl").toUrl();
    info.dateModified_ = value(variant, "dateModified").toDateTime();
    info.parentGameCategoryId_ = value(variant, "parentGameCategoryId").toInt();
    info.rootGameCategoryId_ = value(variant, "rootGameCategoryId").toInt();
    info.url_ = value(variant, "url").toUrl();

    static QMap<QString, QString> replaceMap{
        { "Map and Information", QObject::tr("Map and Information") },
        { "Addons", QObject::tr("Addons") },
        { "Armor, Tools, and Weapons", QObject::tr("Armor, Tools, and Weapons") },
        { "Structures", QObject::tr("Structures") },
        { "Blood Magic", QObject::tr("Blood Magic") },
        { "Industrial Craft", QObject::tr("Industrial Craft") },
        { "Twitch Integration", QObject::tr("Twitch Integration") },
        { "Tinker's Construct", QObject::tr("Tinker's Construct") },
        { "Magic", QObject::tr("Magic") },
        { "Storage", QObject::tr("Storage") },
        { "Thaumcraft", QObject::tr("Thaumcraft") },
        { "Lucky Blocks", QObject::tr("Lucky Blocks") },
        { "Forestry", QObject::tr("Forestry") },
        { "Technology", QObject::tr("Technology") },
        { "Player Transport", QObject::tr("Player Transport") },
        { "Energy, Fluid, and Item Transport", QObject::tr("Energy, Fluid, and Item Transport") },
        { "Buildcraft", QObject::tr("Buildcraft") },
        { "Genetics", QObject::tr("Genetics") },
        { "Ores and Resources", QObject::tr("Ores and Resources") },
        { "Processing", QObject::tr("Processing") },
        { "CraftTweaker", QObject::tr("CraftTweaker") },
        { "Farming", QObject::tr("Farming") },
        { "Adventure and RPG", QObject::tr("Adventure and RPG") },
        { "Cosmetic", QObject::tr("Cosmetic") },
        { "Applied Energistics 2", QObject::tr("Applied Energistics 2") },
        { "API and Library", QObject::tr("API and Library") },
        { "Energy", QObject::tr("Energy") },
        { "Miscellaneous", QObject::tr("Miscellaneous") },
        { "Server Utility", QObject::tr("Server Utility") },
        { "Thermal Expansion", QObject::tr("Thermal Expansion") },
        { "Dimensions", QObject::tr("Dimensions") },
        { "Mobs", QObject::tr("Mobs") },
        { "Biomes", QObject::tr("Biomes") },
        { "Redstone", QObject::tr("Redstone") },
        { "World Gen", QObject::tr("World Gen") },
        { "Food", QObject::tr("Food") },
        { "Automation", QObject::tr("Automation") },
        { "FancyMenu", QObject::tr("FancyMenu") },
        { "MCreator", QObject::tr("MCreator") },
        { "Utility & QOL", QObject::tr("Utility & QOL") },
        { "Fabric", QObject::tr("Fabric") },
        { "Vanilla+", QObject::tr("Vanilla+") },
        { "QoL", QObject::tr("QoL") },
        { "Utility & QoL", QObject::tr("Utility & QoL") }
    };
    if(replaceMap.contains(info.name_))
        info.name_ = replaceMap.value(info.name_);
    else
        qDebug() << "Untranslated category:" << info.name_;

    return info;
}

int CurseforgeCategoryInfo::id() const
{
    return id_;
}

const QString &CurseforgeCategoryInfo::name() const
{
    return name_;
}

const QString &CurseforgeCategoryInfo::slug() const
{
    return slug_;
}

const QUrl &CurseforgeCategoryInfo::avatarUrl() const
{
    return avatarUrl_;
}

const QDateTime &CurseforgeCategoryInfo::dateModified() const
{
    return dateModified_;
}

int CurseforgeCategoryInfo::parentGameCategoryId() const
{
    return parentGameCategoryId_;
}

int CurseforgeCategoryInfo::rootGameCategoryId() const
{
    return rootGameCategoryId_;
}

const QUrl &CurseforgeCategoryInfo::url() const
{
    return url_;
}
