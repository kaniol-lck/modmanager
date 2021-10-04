#include "tag.h"

#include "util/tutil.hpp"
#include <QHash>
#include <QDebug>

QSet<Tag> Tag::functionalityTags_{};
QSet<Tag> Tag::customTags_{};

Tag::Tag(const QString &name, const TagCategory &tagCategory, const QString &iconName) :
    tagCategory_(tagCategory),
    name_(name),
    iconName_(iconName)
{
    if(tagCategory == TagCategory::FunctionalityCategory)
        functionalityTags_ << *this;
    if(tagCategory == TagCategory::CustomCategory)
        customTags_ << *this;
}

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
    Tag tag(value(variant, "name").toString(), TagCategory::fromId(value(variant, "category").toString()));
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

const QSet<Tag> &Tag::functionalityTags()
{
    return functionalityTags_;
}

const QSet<Tag> &Tag::customTags()
{
    return customTags_;
}

const QString &Tag::iconName() const
{
    return iconName_;
}

uint qHash(const Tag &key, uint seed)
{
    return qHash(key.name(), seed);
}
