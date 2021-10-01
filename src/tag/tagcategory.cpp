#include "tagcategory.h"

#include <QObject>

const TagCategory TagCategory::CurseforgeCategory = TagCategory("curseforge", QColor("#f16436"));
const TagCategory TagCategory::ModrinthCategory = TagCategory("modrinth", QColor("#5DA426"));
const TagCategory TagCategory::OptiFineCategory = TagCategory("optifine", QColor(126, 40, 41));
const TagCategory TagCategory::TypeCategory = TagCategory("type", QColor(95, 95, 95));
const TagCategory TagCategory::TranslationCategory = TagCategory("translation", QColor(47, 95, 127));
const TagCategory TagCategory::FunctionalityCategory = TagCategory("functionality", QColor(79, 152, 249));
const TagCategory TagCategory::NotationCategory = TagCategory("notation", QColor(23, 127, 191));
const TagCategory TagCategory::CustomCategory = TagCategory("custom", QColor(127, 63, 23));
const QList<TagCategory> TagCategory::PresetCategories{
    CurseforgeCategory,
    ModrinthCategory,
    OptiFineCategory,
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
