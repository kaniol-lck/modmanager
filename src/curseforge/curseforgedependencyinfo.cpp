#include "curseforgedependencyinfo.h"

#include "util/tutil.hpp"
#include "curseforgefileinfo.h"

CurseforgeDependencyInfo CurseforgeDependencyInfo::fromVariant(const QVariant &variant)
{
    CurseforgeDependencyInfo info;
    info.addonId_ = value(variant, "addonId").toInt();
    info.fileId_ = value(variant, "fileId").toInt();
    info.id_ = value(variant, "id").toInt();
    info.type_ = static_cast<Type>(value(variant, "type").toInt());
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

QString CurseforgeDependencyInfo::typeString() const
{
    switch (type_) {
    case CurseforgeDependencyInfo::EmbeddedLibrary:
        return QObject::tr("Embedded Library");
    case CurseforgeDependencyInfo::Incompatible:
        return QObject::tr("Incompatible");
    case CurseforgeDependencyInfo::OptionalDependency:
        return QObject::tr("Optional Dependency");
    case CurseforgeDependencyInfo::RequiredDependency:
        return QObject::tr("Required Dependency");
    case CurseforgeDependencyInfo::Tool:
        return QObject::tr("Tool");
    }
}
