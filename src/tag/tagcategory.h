#ifndef TAGCATEGORY_H
#define TAGCATEGORY_H

#include <QColor>

class TagCategory
{
public:
    explicit TagCategory(const QString &id, const QColor &color);
    const QColor &color() const;

    bool operator==(const TagCategory &other) const;
    bool operator!=(const TagCategory &other) const;
    bool operator<(const TagCategory &other) const;

    bool isCustomizable() const;

    static TagCategory fromId(const QString &id);

    static const TagCategory SubDirCategory;
    static const TagCategory FileNameCategory;
    static const TagCategory EnvironmentCategory;
    static const TagCategory LanguageCategory;
    static const TagCategory CurseforgeCategory;
    static const TagCategory ModrinthCategory;
    static const TagCategory OptiFineCategory;
    static const TagCategory TypeCategory;
    static const TagCategory TranslationCategory;
    static const TagCategory FunctionalityCategory;
    static const TagCategory NotationCategory;
    static const TagCategory CustomCategory;

    static const QList<TagCategory> PresetCategories;
    static const QList<TagCategory> FilterCategories;
    static const QList<TagCategory> CustomizableCategories;

    const QString &id() const;
    QString name() const;

private:
    QString id_;
    QColor color_;
};

[[maybe_unused]]
uint qHash(const TagCategory &key, uint seed = 0);

#endif // TAGCATEGORY_H
