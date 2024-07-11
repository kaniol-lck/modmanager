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

QString styleSheetPath(QString name)
{
    if(!styleSheets().keys().contains(name))
        name = "basic";
    if(builtinStyleSheets().keys().contains(name)){
        QFile f(QString(":/stylesheet/%1.qss").arg(name));
        if(f.open(QIODevice::ReadOnly))
            return f.readAll();
    }
    auto fileName = QDir(styleSheetsPath()).absoluteFilePath(name + ".qss");
    return fileName.prepend("file:///");
}

QString copyStyleSheet(const QString &name)
{
    QFile oldFile;
    if(builtinStyleSheets().keys().contains(name))
        oldFile.setFileName(QString(":/stylesheet/%1.qss").arg(name));
    else
        oldFile.setFileName(QString("file:///:/stylesheet/%1.qss").arg(name));
    auto newName = name+"-copy";
    auto fileName = QDir(styleSheetsPath()).absoluteFilePath(newName + ".qss");
    QFile newFile(fileName);
    if(!newFile.exists() && oldFile.open(QIODevice::ReadOnly) && newFile.open(QIODevice::WriteOnly)){
        newFile.write("/* This file is auto generated from existed stylesheet\n"
                      " * you can create you own qss based on this file.\n"
                      " */\n\n");
        newFile.write(oldFile.readAll());
    }
    return newName;
}
