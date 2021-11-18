#include "tagcategory.h"

#include <QObject>

const TagCategory TagCategory::SubDirCategory = TagCategory("sub-dir", QColor(34, 161, 181));
const TagCategory TagCategory::EnvironmentCategory = TagCategory("environment", QColor(124, 143, 93));
const TagCategory TagCategory::CurseforgeCategory = TagCategory("curseforge", QColor(241, 100, 54));
const TagCategory TagCategory::ModrinthCategory = TagCategory("modrinth", QColor(93, 164, 38));
const TagCategory TagCategory::OptiFineCategory = TagCategory("optifine", QColor(126, 40, 41));
const TagCategory TagCategory::TypeCategory = TagCategory("type", QColor(95, 95, 95));
const TagCategory TagCategory::TranslationCategory = TagCategory("translation", QColor(47, 95, 127));
const TagCategory TagCategory::FunctionalityCategory = TagCategory("functionality", QColor(79, 152, 249));
const TagCategory TagCategory::NotationCategory = TagCategory("notation", QColor(23, 127, 191));
const TagCategory TagCategory::CustomCategory = TagCategory("custom", QColor(127, 63, 23));
const QList<TagCategory> TagCategory::PresetCategories{
    SubDirCategory,
    EnvironmentCategory,
    CurseforgeCategory,
    ModrinthCategory,
    OptiFineCategory,
    TypeCategory,
    TranslationCategory,
    FunctionalityCategory,
    NotationCategory,
    CustomCategory
};

const QList<TagCategory> TagCategory::CustomizableCategories{
//    EnvironmentCategory,
//    CurseforgeCategory,
//    ModrinthCategory,
//    OptiFineCategory,
    TypeCategory,
    TranslationCategory,
    FunctionalityCategory,
    NotationCategory,
    CustomCategory
};

TagCategory::TagCategory(const QString &id, const QColor &color) :
    id_(id),
    color_(color)
{}

const QColor &TagCategory::color() const
{
    return color_;
}

bool TagCategory::operator==(const TagCategory &other) const
{
    return id_ == other.id_;
}

bool TagCategory::operator!=(const TagCategory &other) const
{
    return !operator==(other);
}

TagCategory TagCategory::fromId(const QString &id)
{
    for(auto category : PresetCategories)
        if(id == category.id())
            return category;
    return TagCategory(id, Qt::gray);
}

const QString &TagCategory::id() const
{
    return id_;
}

QString TagCategory::name() const
{
    if(id_ == "sub-dir")
        return QObject::tr("Sub-Directory Tag");
    if(id_ == "environment")
        return QObject::tr("Environment Tag");
    if(id_ == "curseforge")
        return QObject::tr("Curseforge Tag");
    if(id_ == "modrinth")
        return QObject::tr("Modrinth Tag");
    if(id_ == "optifine")
        return QObject::tr("Optifine Tag");
    if(id_ == "type")
        return QObject::tr("Type Tag");
    if(id_ == "translation")
        return QObject::tr("Translation Tag");
    if(id_ == "functionality")
        return QObject::tr("Functionality Tag");
    if(id_ == "notation")
        return QObject::tr("Notation Tag");
    if(id_ == "custom")
        return QObject::tr("Custom Tag");
    return "";
}
