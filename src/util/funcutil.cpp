#include "funcutil.h"

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
#include <QDir>
#include <QProcess>

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"

QString numberConvert(int size, const QString &suffix, int prec, int limit){
    if(size < limit)
        return QString::number(size, 'g', prec) + suffix;
    else if(size < limit * 1000)
        return QString::number(size / 1000., 'g', prec) + "K" + suffix;
    else if(size < limit * 1000000)
        return QString::number(size / 1000000., 'g', prec) + "M" + suffix;
    else
        return QString::number(size / 1000000., 'g', prec) + "G" + suffix;
}

void openFileInFolder(const QString &filePath)
{
    QFileInfo info(filePath);
    QString path = QDir(filePath).absolutePath();
    //TODO: does not work on windows
//#if defined(Q_OS_WIN)
//    QStringList args;
//    if (!info.isFile())
//        args << "/select,";
//    args << QDir::toNativeSeparators(path);
//    if (QProcess::startDetached("explorer", args))
//        return;
//#elif defined(Q_OS_MAC)
//    QStringList args;
//    args << "-e";
//    args << "tell application \"Finder\"";
//    args << "-e";
//    args << "activate";
//    args << "-e";
//    args << "select POSIX file \"" + path + "\"";
//    args << "-e";
//    args << "end tell";
//    args << "-e";
//    args << "return";
//    if (!QProcess::execute("/usr/bin/osascript", args))
//        return;
//#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isFile()?info.absolutePath() : path));
}

bool hasFile(const QString &path, const QString &fileName)
{
    //TODO: how to know if is a renamed mod file
    for(const auto &fileInfo : QDir(path).entryInfoList(QDir::Files))
        if(fileInfo.fileName().contains(fileName))
            return true;
    return false;
}

bool hasFile(LocalModPath *path, CurseforgeMod *mod)
{
    for(auto localMod : path->modMap())
        if(localMod->curseforgeMod() && localMod->curseforgeMod()->modInfo().id() == mod->modInfo().id())
            return true;
    return false;
}

bool hasFile(LocalModPath *path, const CurseforgeFileInfo &fileInfo)
{
    //TODO: old file
    if(hasFile(path->info().path(), fileInfo.fileName())) return true;
    for(auto localMod : path->modMap())
        if(localMod->curseforgeMod() && localMod->curseforgeUpdate().currentFileInfo() &&
                localMod->curseforgeUpdate().currentFileInfo()->id() == fileInfo.id())
            return true;
    return false;
}

bool hasFile(LocalModPath *path, ModrinthMod *mod)
{
    for(auto localMod : path->modMap())
        if(localMod->modrinthMod() && localMod->modrinthMod()->modInfo().id() == mod->modInfo().id())
            return true;
    return false;
}

bool hasFile(LocalModPath *path, const ModrinthFileInfo &fileInfo)
{
    //TODO: old file
    if(hasFile(path->info().path(), fileInfo.fileName())) return true;
    for(auto localMod : path->modMap())
        if(localMod->modrinthMod() && localMod->modrinthUpdate().currentFileInfo() &&
                localMod->modrinthUpdate().currentFileInfo()->id() == fileInfo.id())
            return true;
    return false;
}

QString capture(const QString &str, const QString &regExp, bool minimal, int offset)
{
    QRegExp re(regExp);
    re.setMinimal(minimal);
    if(re.indexIn(str, offset) != -1)
        return re.cap(1);
    else
        return "";
}
