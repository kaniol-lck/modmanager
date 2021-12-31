#include "curseforgedependencyinfo.h"

#include "util/tutil.hpp"
#include "curseforgefileinfo.h"

CurseforgeDependencyInfo CurseforgeDependencyInfo::fromVariant(const QVariant &variant)
{
    CurseforgeDependencyInfo info;
    info.addonId_ = value(variant, "addonId").toInt();
    info.fileId_ = value(variant, "fileId").toInt();
    info.id_ = value(variant, "id").toInt();
    info.type_ = value(variant, "type").toInt();
    return info;
}

int CurseforgeDependencyInfo::addonId() const
{
    return addonId_;
}

int CurseforgeDependencyInfo::fileId() const
{
    return fileId_;
}

int CurseforgeDependencyInfo::id() const
{
    return id_;
}

int CurseforgeDependencyInfo::type() const
{
    return type_;
}
