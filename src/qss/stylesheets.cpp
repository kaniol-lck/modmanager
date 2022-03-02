#include "stylesheets.h"

#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

const QString &styleSheetsPath()
{
    static auto path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
            .absoluteFilePath("stylesheets");
    QDir().mkpath(path);
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
    auto &&fileInfoList = QDir(styleSheetsPath()).entryInfoList();
    for(auto &&fileInfo : fileInfoList)
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
    if(!QFileInfo::exists(fileName) && builtinStyleSheets().contains(name)){
        QFile builtinFile(QString(":/stylesheet/%1.qss").arg(name));
        QFile newFile(fileName);
        if(builtinFile.open(QIODevice::ReadOnly) && newFile.open(QIODevice::WriteOnly)){
            newFile.write("/* This file is auto generated from built-in stylesheet\n"
                          " * you can create you own qss based on this file.\n"
                          " */\n\n");
            newFile.write(builtinFile.readAll());
        }
    }
    return fileName.prepend("file:///");
}
