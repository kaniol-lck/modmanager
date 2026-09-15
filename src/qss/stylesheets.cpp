#include "stylesheets.h"
#include "config.hpp"

#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

QColor themeColor(ThemeColor role)
{
    //自定义 qss 没有「深色/浅色」的元信息，一律按浅色处理（与原来写死的浅色取值一致）
    const bool dark = Config().getCustomStyle() == QLatin1String("dark");
    switch(role){
    case ThemeColor::UpdateAvailable:
        //原实现写死 darkgreen，在深色主题下对比度只有约 1.3:1，几乎读不出来
        return dark ? QColor(0x5c, 0xc9, 0x8a) : QColor(Qt::darkGreen);
    }
    return QColor();
}

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
        //basic 只覆盖了部分控件（没有按钮/下拉/菜单/工具栏/分组框/Tab 的样式），
        //选中时这些控件会回落到 Qt 原生外观，与 light/dark 观感不一致，故在名称上说明
        { "basic", QObject::tr("Basic (incomplete)") },
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
        //自定义样式存在可写目录里（原来误写成 "file:///:/stylesheet/..." ，混合了 file:// 与资源前缀，必然打不开）
        oldFile.setFileName(QDir(styleSheetsPath()).absoluteFilePath(name + ".qss"));
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
