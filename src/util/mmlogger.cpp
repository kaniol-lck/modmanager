#include "mmlogger.h"

#include <QDebug>

MMLogger::MMLogger(const QString &context, const QString &field) :
    ts_(stdout),
    context_(context),
    field_(field)
{
    ts_ << "Logger " << context_ << " - " << field_ << " : ";
}

MMLogger::~MMLogger()
{
    ts_ << Qt::endl;
}

MMLogger MMLogger::dector(const QString &field)
{
    return MMLogger("DECTOR", field);
}

MMLogger MMLogger::dector(QObject *object)
{
    return MMLogger::dector(object->metaObject()->className());
}
