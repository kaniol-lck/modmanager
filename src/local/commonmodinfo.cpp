#include "commonmodinfo.h"

const QString &CommonModInfo::id() const
{
    return id_;
}

const QString &CommonModInfo::name() const
{
    return name_;
}

const QString &CommonModInfo::version() const
{
    return version_;
}

const QStringList &CommonModInfo::authors() const
{
    return authors_;
}

const QString &CommonModInfo::description() const
{
    return description_;
}

const QByteArray &CommonModInfo::iconBytes() const
{
    return iconBytes_;
}

const QUrl &CommonModInfo::homepage() const
{
    return homepage_;
}

const QUrl &CommonModInfo::sources() const
{
    return sources_;
}

const QUrl &CommonModInfo::issues() const
{
    return issues_;
}

const QStringList &CommonModInfo::languages() const
{
    return languages_;
}
