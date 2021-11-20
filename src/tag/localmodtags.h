#ifndef LOCALMODTAGS_H
#define LOCALMODTAGS_H

#include <QList>
#include <optional>

#include "tag.h"

class LocalModTags
{
public:
    LocalModTags() = default;

    void addTag(const Tag &tag);
    void removeTag(const Tag &tag);
    void removeTags(const QList<TagCategory> &categories = TagCategory::PresetCategories);
    LocalModTags &operator<<(const Tag &tag);

    QList<Tag> tags(const QList<TagCategory> &categories = TagCategory::PresetCategories) const;
    const QList<Tag> &subDirTags() const;
    const QList<Tag> &fileNameTags() const;
    const QList<Tag> &environmentTags() const;
    const QList<Tag> &typeTags() const;
    const QList<Tag> &functionalityTags() const;
    std::optional<Tag> translationTag() const;
    std::optional<Tag> notationTag() const;
    const QList<Tag> &customTags() const;
private:
    QList<Tag> subDirTags_;
    QList<Tag> fileNameTags_;
    QList<Tag> environmentTags_;
    QList<Tag> typeTags_;
    QList<Tag> functionalityTags_;
    std::optional<Tag> translationTag_;
    std::optional<Tag> notationTag_;
    QList<Tag> customTags_;
};

#endif // LOCALMODTAGS_H
