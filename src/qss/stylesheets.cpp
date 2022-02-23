#include "stylesheets.h"

#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

const QString &styleSheetsPath()
{
    static auto path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
            .absoluteFilePath("stylesheets");
    return path;
}

const QMap<QString, QString> &builtinStyleSheets()
{
    static const QMap<QString, QString> stylesheets{
        { "basic", QObject::tr("Basic") },
        { "light", QObject::tr("Light") },
        { "dark", QObject::tr("Dark") }
    };
    return stylesheets;
}

QMap<QString, QString> styleSheets()
{
    auto styleSheets = builtinStyleSheets();
    for(auto &&fileInfo : QDir(styleSheetsPath()).entryInfoList())
        if(fileInfo.suffix() == "qss"){
            auto name = fileInfo.baseName();
            if(!styleSheets.contains(name))
                styleSheets.insert(name, name);
        }
    return styleSheets;
}

QString styleSheetPath(const QString &name)
{
    if(!styleSheets().contains(name))
        return QString("file:///:/stylesheet/%1.qss").arg("basic");
    auto fileName = QDir(styleSheetsPath()).absoluteFilePath(name + ".qss");
    if(!QFileInfo(fileName).exists() && builtinStyleSheets().contains(name))
        return QString("file:///:/stylesheet/%1.qss").arg(name);
    return fileName;
}
