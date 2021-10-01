#ifndef TAGCATEGORY_H
#define TAGCATEGORY_H

#include <QColor>

class TagCategory
{
public:
    enum class Type{
        CurseforgeCategory,
        ModrinthCategory,
        OptiFineCategory,
        TypeCategory,
        TranslationCategory,
        FunctionalityCategory,
        CustomCategory
    };
    explicit TagCategory(const QString &id, const QColor &color, Type type = Type::CustomCategory);
    QString name() const;
    const QColor &color() const;

    bool operator==(const TagCategory &other) const;

    static TagCategory fromId(const QString &id);

    static const TagCategory CurseforgeCategory;
    static const TagCategory ModrinthCategory;
    static const TagCategory OptiFineCategory;
    static const TagCategory TypeCategory;
    static const TagCategory TranslationCategory;
    static const TagCategory FunctionalityCategory;
    static const TagCategory CustomCategory;
    static const QList<TagCategory> PresetCategories;

    const QString &id() const;

private:
    QString id_;
    QColor color_;
    Type  type_ = Type::CustomCategory;
};

#endif // TAGCATEGORY_H
