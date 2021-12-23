#include "curseforgecategoryinfo.h"

#include "util/tutil.hpp"

CurseforgeCategoryInfo CurseforgeCategoryInfo::fromVariant(const QVariant &variant)
{
    CurseforgeCategoryInfo info;
    info.id_ = value(variant, "id").toInt();
    info.name_ = value(variant, "name").toString();
    info.slug_ = value(variant, "slug").toString();
    info.avatarUrl_ = value(variant, "avatarUrl").toUrl();
    info.dateModified_ = value(variant, "dateModified").toDateTime();
    info.parentGameCategoryId_ = value(variant, "parentGameCategoryId").toInt();
    info.rootGameCategoryId_ = value(variant, "rootGameCategoryId").toInt();
    return info;
}

int CurseforgeCategoryInfo::id() const
{
    return id_;
}

const QString &CurseforgeCategoryInfo::name() const
{
    return name_;
}

const QString &CurseforgeCategoryInfo::slug() const
{
    return slug_;
}

const QUrl &CurseforgeCategoryInfo::avatarUrl() const
{
    return avatarUrl_;
}

const QDateTime &CurseforgeCategoryInfo::dateModified() const
{
    return dateModified_;
}

int CurseforgeCategoryInfo::parentGameCategoryId() const
{
    return parentGameCategoryId_;
}

int CurseforgeCategoryInfo::rootGameCategoryId() const
{
    return rootGameCategoryId_;
}
