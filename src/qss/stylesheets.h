#ifndef STYLESHEETS_H
#define STYLESHEETS_H
#include <QMap>

const QMap<QString, QString> &styleSheets();

QString styleSheetPath(const QString &name);
#endif // STYLESHEETS_H
