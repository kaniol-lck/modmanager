#ifndef STYLESHEETS_H
#define STYLESHEETS_H
#include <QMap>

const QString &styleSheetsPath();

const QMap<QString, QString> &builtinStyleSheets();
QMap<QString, QString> styleSheets();

QString styleSheetPath(QString name);
QString copyStyleSheet(const QString &name);
#endif // STYLESHEETS_H
