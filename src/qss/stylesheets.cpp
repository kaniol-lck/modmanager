#include "stylesheets.h"

#include <QObject>

const QMap<QString, QString> &styleSheets()
{
    static const QMap<QString, QString> stylesheets{
        { "basic", QObject::tr("Basic") },
        { "light", QObject::tr("Light") },
        { "dark", QObject::tr("Dark") }
    };
    return stylesheets;
}

QString styleSheetPath(const QString &name)
{
    if(!styleSheets().contains(name))
        return QString("file:///:/stylesheet/%1.qss").arg("basic");
    return QString("file:///:/stylesheet/%1.qss").arg(name);
}
