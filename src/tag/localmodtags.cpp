#include "localmodtags.h"

void LocalModTags::addTag(const Tag &tag)
{
    auto category = tag.tagCategory();
    if(category == TagCategory::SubDirCategory){
        if(!subDirTags_.contains(tag))
            subDirTags_ << tag;
    }else if(category == TagCategory::EnvironmentCategory){
        if(!environmentTags_.contains(tag))
            environmentTags_ << tag;
    }else if(category == TagCategory::TypeCategory){
        if(!typeTags_.contains(tag))
            typeTags_ << tag;
    }else if(category == TagCategory::FunctionalityCategory){
        if(!functionalityTags_.contains(tag))
            functionalityTags_ << tag;
    }else if(category == TagCategory::TranslationCategory)
        translationTag_.emplace(tag);
    else if(category == TagCategory::NotationCategory)
        notationTag_.emplace(tag);
    else if(category == TagCategory::CustomCategory){
        if(!customTags_.contains(tag))
            customTags_ << tag;
    }
}

void LocalModTags::removeTag(const Tag &tag)
{
    auto category = tag.tagCategory();
    if(category == TagCategory::TypeCategory)
        typeTags_.removeAll(tag);
    else if(category == TagCategory::FunctionalityCategory)
        functionalityTags_.removeAll(tag);
    else if(category == TagCategory::TranslationCategory)
        translationTag_.reset();
    else if(category == TagCategory::NotationCategory)
        notationTag_.reset();
    else if(category == TagCategory::CustomCategory)
        customTags_.removeAll(tag);
}

void LocalModTags::removeTags(const QList<TagCategory> &categories)
{
    for(const auto &category : categories){
        if(category == TagCategory::SubDirCategory)
            subDirTags_.clear();
        if(category == TagCategory::EnvironmentCategory)
            environmentTags_.clear();
        if(category == TagCategory::TypeCategory)
            typeTags_.clear();
        else if(category == TagCategory::FunctionalityCategory)
            functionalityTags_.clear();
        else if(category == TagCategory::TranslationCategory)
            translationTag_.reset();
        else if(category == TagCategory::NotationCategory)
            notationTag_.reset();
        else if(category == TagCategory::CustomCategory)
            customTags_.clear();
    }
}

LocalModTags &LocalModTags::operator<<(const Tag &tag)
{
    addTag(tag);
    return *this;
}

QList<Tag> LocalModTags::tags(const QList<TagCategory> &categories) const
{
    QList<Tag> list;
    for(const auto &category : categories){
        if(category == TagCategory::SubDirCategory)
            list << subDirTags_;
        if(category == TagCategory::EnvironmentCategory)
            list << environmentTags_;
        if(category == TagCategory::TypeCategory)
            list << typeTags_;
        else if(category == TagCategory::FunctionalityCategory)
            list << functionalityTags_;
        else if(category == TagCategory::TranslationCategory){
            if(translationTag_) list << *translationTag_;
        } else if(category == TagCategory::NotationCategory){
            if(notationTag_) list << *notationTag_;
        } else if(category == TagCategory::CustomCategory)
            list << customTags_;
    }
    return list;
}

const QList<Tag> &LocalModTags::typeTags() const
{
    return typeTags_;
}

const QList<Tag> &LocalModTags::functionalityTags() const
{
    return functionalityTags_;
}

std::optional<Tag> LocalModTags::translationTag() const
{
    return translationTag_;
}

std::optional<Tag> LocalModTags::notationTag() const
{
    return notationTag_;
}

const QList<Tag> &LocalModTags::customTags() const
{
    return customTags_;
}

const QList<Tag> &LocalModTags::subDirTags() const
{
    return subDirTags_;
}

const QList<Tag> &LocalModTags::environmentTags() const
{
    return environmentTags_;
}
