#include "tag.h"

#include "util/tutil.hpp"
#include <QObject>

Tag::Tag(const QString &name, const TagCategory &tagCategory) :
    tagCategory_(tagCategory),
    name_(name)
{}

bool Tag::operator==(const Tag &other) const
{
    return name_ == other.name_ &&
           tagCategory_ == other.tagCategory_;
}

QJsonValue Tag::toJsonValue() const
{
    if(tagCategory_ == TagCategory::CustomCategory)
        return name_;
    QJsonObject object;
    object.insert("name", name_);
    object.insert("category", tagCategory_.id());
    return object;
}

Tag Tag::fromVariant(const QVariant &variant)
{
    if(auto name = variant.toString(); !name.isEmpty())
        return Tag(name);
    Tag tag;
    tag.name_ = value(variant, "name").toString();
    tag.tagCategory_ = TagCategory::fromId(value(variant, "category").toString());
    return tag;
}

const TagCategory &Tag::tagCategory() const
{
    return tagCategory_;
}

const QString &Tag::name() const
{
    return name_;
}

const QList<Tag> &Tag::typeTags()
{
    static QList<Tag> typeTags{
        Tag(QObject::tr("Library"), TagCategory::TypeCategory),
        Tag(QObject::tr("Addon"), TagCategory::TypeCategory)
    };
    return typeTags;
}
