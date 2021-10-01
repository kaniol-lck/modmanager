#include "tagcategory.h"

#include <QObject>

const TagCategory TagCategory::CurseforgeCategory = TagCategory("curseforge", QColor("#f16436"), TagCategory::Type::CurseforgeCategory);
const TagCategory TagCategory::ModrinthCategory = TagCategory("modrinth", QColor("#5DA426"), TagCategory::Type::ModrinthCategory);
const TagCategory TagCategory::OptiFineCategory = TagCategory("optifine", QColor(126, 40, 41), TagCategory::Type::OptiFineCategory);
const TagCategory TagCategory::TypeCategory = TagCategory("type", QColor(186, 184, 185), TagCategory::Type::TypeCategory);
const TagCategory TagCategory::TranslationCategory = TagCategory("translation", QColor(47, 95, 127), TagCategory::Type::TranslationCategory);
const TagCategory TagCategory::FunctionalityCategory = TagCategory("functionality", QColor(79, 152, 249), TagCategory::Type::FunctionalityCategory);
const TagCategory TagCategory::CustomCategory = TagCategory("custom", QColor(23, 127, 191), TagCategory::Type::CustomCategory);

TagCategory::TagCategory(const QString &id, const QColor &color, Type type) :
    id_(id),
    color_(color),
    type_(type)
{}

QString TagCategory::name() const
{
    switch (type_) {
    case Type::CurseforgeCategory:
        return "Curseforge";
        break;
    case Type::ModrinthCategory:
        break;
    case Type::OptiFineCategory:
        break;
    case Type::TypeCategory:
        break;
    case Type::TranslationCategory:
        break;
    case Type::FunctionalityCategory:
        break;
    case Type::CustomCategory:
        return QObject::tr("Custom");
        break;
    }
    return id_;
}

const QColor &TagCategory::color() const
{
    return color_;
}

bool TagCategory::operator==(const TagCategory &other) const
{
    return type_ == other.type_;
}

TagCategory TagCategory::fromId(const QString &id)
{
    if(id == CurseforgeCategory.id())
        return CurseforgeCategory;
    if(id == TranslationCategory.id())
        return TranslationCategory;
    if(id == CustomCategory.id())
        return CustomCategory;
    return TagCategory(id, Qt::gray);
}

const QString &TagCategory::id() const
{
    return id_;
}
