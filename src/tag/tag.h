#ifndef TAG_H
#define TAG_H

#include <QJsonObject>

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

private:
    TagCategory tagCategory_ = TagCategory::CustomCategory;
    QString name_;
};

#endif // TAG_H
