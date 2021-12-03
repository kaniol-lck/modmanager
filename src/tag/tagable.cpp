#include "tagable.h"

void Tagable::addTag(const Tag &tag)
{
    auto category = tag.category();
    if(tagMap_.contains(category))
        tagMap_[category] << tag;
    else
        tagMap_[category] = { tag };
}

void Tagable::addTags(const TagCategory &category, const QList<Tag> &tags)
{
    if(tagMap_.contains(category))
        tagMap_[category] << tags;
    else
        tagMap_[category] = tags;
}

void Tagable::removeTag(const Tag &tag)
{
    auto category = tag.category();
    tagMap_[category].removeAll(tag);
}

void Tagable::removeTags(const QList<TagCategory> &categories)
{
    for(const auto &category : categories)
        tagMap_.remove(category);
}

void Tagable::removeTags(const TagCategory &category)
{
    tagMap_.remove(category);
}

Tagable &Tagable::operator<<(const Tag &tag)
{
    addTag(tag);
    return *this;
}

Tagable &Tagable::operator<<(const Tagable &tags)
{
    for(auto it = tags.tagMap_.begin(); it != tags.tagMap_.end(); it++)
        addTags(it.key(), it.value());
    return *this;
}

QList<Tag> Tagable::tags(const QList<TagCategory> &categories, bool self) const
{
    QList<Tag> list;
    for(const auto &category : categories)
        list << tags(category, self);
    return list;
}

QList<Tag> Tagable::tags(const TagCategory &category, bool self) const
{
    if(self)
        return tagMap_.value(category, {});
    else{
        auto list = tagMap_.value(category, {});
        for(const auto &child : children_)
            for(auto &&tag : child->tags(category))
                if(!list.contains(tag))
                    list << tag;
        return list;
    }
}

QList<Tag> Tagable::customizableTags() const
{
    return tags(TagCategory::CustomizableCategories);
}

void Tagable::addSubTagable(Tagable *child)
{
    children_ << child;
}

void Tagable::removeSubTagable(Tagable *child)
{
    children_.removeAll(child);
}
