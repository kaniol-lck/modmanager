#include "mmlogger.h"

#include <QDebug>

MMLogger::MMLogger(const QString &context, const QString &field, bool mute) :
    ts_(stdout),
    context_(context),
    field_(field),
    mute_(mute)
{
    if(!mute_)
        ts_ << "[LOG] " << context_ << " - " << field_ << " : ";
}

MMLogger::~MMLogger()
{
    if(!mute_)
        ts_ << Qt::endl;
}

MMLogger MMLogger::dtor(const QString &field)
{
    return MMLogger("DTOR", field, true);
}

MMLogger MMLogger::dtor(QObject *object)
{
    return MMLogger::dtor(object->metaObject()->className());
}

MMLogger MMLogger::network(const QString &field)
{
    return MMLogger("NETWORK", field, true);
}

MMLogger MMLogger::network(QObject *object)
{
    return MMLogger::network(object->metaObject()->className());
}

MMLogger &MMLogger::operator<<(const QUrl &url)
{
    if(!mute_)
        ts_ << url.toString();
    return *this;
}
