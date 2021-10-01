#ifndef TAG_H
#define TAG_H

#include <QJsonObject>
#include <QHash>

#include "tagcategory.h"

class Tag
{
public:
    Tag() = default;
    explicit Tag(const QString &name, const TagCategory &tagCategory = TagCategory::CustomCategory);

    bool operator==(const Tag &other) const;

    QJsonValue toJsonValue() const;
    static Tag fromVariant(const QVariant &variant);

    const TagCategory &tagCategory() const;
    const QString &name() const;

    static const QList<Tag> &typeTags();
    static const QSet<Tag> &functionalityTags();
    static const QSet<Tag> &customTags();

private:
    TagCategory tagCategory_ = TagCategory::CustomCategory;
    QString name_;

    static QSet<Tag> functionalityTags_;
    static QSet<Tag> customTags_;
};

[[maybe_unused]]
static uint qHash(const Tag &key, uint seed = 0);

#endif // TAG_H
