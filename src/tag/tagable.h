#ifndef TAGABLE_H
#define TAGABLE_H

#include <QList>
#include <optional>

#include "tag.h"

class Tagable
{
public:
    Tagable() = default;
    ~Tagable();

    //impport will not trigger editted
    void importTag(const Tag &tag);
    void addTag(const Tag &tag);
    void addTags(const TagCategory &category, const QList<Tag> &tag);
    void removeTag(const Tag &tag);
    void removeTags(const QList<TagCategory> &categories = TagCategory::PresetCategories);
    void removeTags(const TagCategory &category);
    Tagable &operator<<(const Tag &tag);
    Tagable &operator<<(const Tagable &tags);

    QList<Tag> tags(const QList<TagCategory> &categories = TagCategory::PresetCategories, bool self = false) const;
    QList<Tag> tags(const TagCategory &category, bool self = false) const;
    QList<Tag> customizableTags() const;

    void addSubTagable(Tagable *child);
    void removeSubTagable(Tagable *child);

protected:
    virtual void tagsChanged();
    virtual void tagsEditted();

private:
    void setTagsChanged();
    void setTagsEditted();

    QMap<TagCategory, QList<Tag>> tagMap_;
    QList<Tagable *> parents_ = {};
    QList<Tagable *> children_ = {};
};

#endif // TAGABLE_H
