#include "tagable.h"

Tagable::~Tagable()
{
    for(auto parent : qAsConst(parents_))
        parent->children_.removeAll(this);
    for(auto child : qAsConst(children_))
        child->parents_.removeAll(this);
}

void Tagable::importTag(const Tag &tag)
{
    auto category = tag.category();
    if(tagMap_.contains(category))
        tagMap_[category] << tag;
    else
        tagMap_[category] = { tag };
    setTagsChanged();
}

void Tagable::addTag(const Tag &tag)
{
    auto category = tag.category();
    if(tagMap_.contains(category))
        tagMap_[category] << tag;
    else
        tagMap_[category] = { tag };
    setTagsChanged();
    if(category.isCustomizable()) setTagsEditted();
}

void Tagable::addTags(const TagCategory &category, const QList<Tag> &tags)
{
    if(tagMap_.contains(category))
        tagMap_[category] << tags;
    else
        tagMap_[category] = tags;
    setTagsChanged();
    if(category.isCustomizable()) setTagsEditted();
}

void Tagable::removeTag(const Tag &tag)
{
    auto category = tag.category();
    tagMap_[category].removeAll(tag);
    setTagsChanged();
    if(category.isCustomizable()) setTagsEditted();
}

void Tagable::removeTags(const QList<TagCategory> &categories)
{
    bool editted= false;
    for(const auto &category : categories){
        tagMap_.remove(category);
        if(category.isCustomizable()) editted = true;
    }
    setTagsChanged();
    if(editted) setTagsEditted();
}

void Tagable::removeTags(const TagCategory &category)
{
    tagMap_.remove(category);
    setTagsChanged();
    if(category.isCustomizable()) setTagsEditted();
    if(category.isCustomizable()) setTagsEditted();
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
    if(!child) return;
    children_ << child;
    child->parents_ << this;
    setTagsChanged();
}

void Tagable::removeSubTagable(Tagable *child)
{
    if(!child) return;
    children_.removeAll(child);
    child->parents_.removeAll(this);
    setTagsChanged();
}

void Tagable::tagsChanged()
{
    if(onTagsChanged_) onTagsChanged_();
}

void Tagable::tagsEditted()
{}

void Tagable::setTagsChanged()
{
    for(const auto &parent : qAsConst(parents_))
        parent->setTagsChanged();
    tagsChanged();
}

void Tagable::setTagsEditted()
{
    tagsEditted();
}

void Tagable::setOnTagsChanged(const std::function<void ()> &newOnTagsChanged)
{
    onTagsChanged_ = newOnTagsChanged;
}
