#ifndef STYLESHEETS_H
#define STYLESHEETS_H
#include <QColor>
#include <QMap>

//少数地方必须由代码给出颜色（QTreeWidgetItem::setForeground 之类 QSS 够不到的场合），
//集中在这里按当前主题取值，避免把颜色写死在业务代码里。
enum class ThemeColor {
    UpdateAvailable,//「有更新可用」这类正向提示
};

QColor themeColor(ThemeColor role);

const QString &styleSheetsPath();

const QMap<QString, QString> &builtinStyleSheets();
QMap<QString, QString> styleSheets();

QString styleSheetPath(QString name);
QString copyStyleSheet(const QString &name);
#endif // STYLESHEETS_H
