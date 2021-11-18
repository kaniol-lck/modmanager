#include "funcutil.h"

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QFileDialog>
#include <QListView>
#include <QTreeView>
#include <QDateTime>

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"

QString sizeConvert(qint64 byte, int prec, int limit)
{
    if(byte < limit)
        return QString::number(byte, 'g', prec) + " B";
    else if(double kbyte = byte / 1024.; kbyte < limit)
        return QString::number(kbyte, 'g', prec) + " KiB";
    else if(double mbyte = kbyte / 1024.; mbyte < limit)
        return QString::number(mbyte, 'g', prec) + " MiB";
    else if(double gbyte = mbyte / 1024.; gbyte < limit)
        return QString::number(gbyte, 'g', prec) + " GiB";
    else
        return QString::number(gbyte, 'g', prec) + " GiB";
}

QString speedConvert(qint64 byte, int prec, int limit)
{
    return sizeConvert(byte, prec, limit) + "/s";
}

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
    for(auto &&fileInfo : QDir(path).entryInfoList(QDir::Files))
        if(fileInfo.fileName().contains(fileName))
            return true;
    return false;
}

bool hasFile(LocalModPath *path, CurseforgeMod *mod)
{
    for(auto &&map : path->modMaps())
        for(const auto &localMod : map)
            if(localMod->curseforgeMod() && localMod->curseforgeMod()->modInfo().id() == mod->modInfo().id())
                return true;
    return false;
}

bool hasFile(LocalModPath *path, const CurseforgeFileInfo &fileInfo)
{
    //TODO: old file
    if(hasFile(path->info().path(), fileInfo.fileName())) return true;
    for(auto &&map : path->modMaps())
        for(const auto &localMod : map)
            if(localMod->curseforgeMod() && localMod->curseforgeUpdate().currentFileInfo() &&
                    localMod->curseforgeUpdate().currentFileInfo()->id() == fileInfo.id())
                return true;
    return false;
}

bool hasFile(LocalModPath *path, ModrinthMod *mod)
{
    for(auto &&map : path->modMaps())
        for(const auto &localMod : map)
            if(localMod->modrinthMod() && localMod->modrinthMod()->modInfo().id() == mod->modInfo().id())
                return true;
    return false;
}

bool hasFile(LocalModPath *path, const ModrinthFileInfo &fileInfo)
{
    //TODO: old file
    if(hasFile(path->info().path(), fileInfo.fileName())) return true;
    for(auto &&map : path->modMaps())
        for(const auto &localMod : map)
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

QStringList getExistingDirectories(QWidget *parent, const QString &caption, const QString &dir)
{
    QFileDialog fileDialog(parent, caption, dir);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOptions(QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
    if(auto listView = fileDialog.findChild<QListView*>("listView"))
        listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    if(auto treeView = fileDialog.findChild<QTreeView*>())
        treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    if (fileDialog.exec() == QDialog::Accepted)
        return fileDialog.selectedFiles();
    else
        return {};
}

QString colorCodeFormat(QString str)
{
    QVector<std::tuple<QString, QString>> replaceList{
        {"§0", R"(</span><span style="color:#000000">)"},
        {"§1", R"(</span><span style="color:#0000AA">)"},
        {"§2", R"(</span><span style="color:#00AA00">)"},
        {"§3", R"(</span><span style="color:#00AAAA">)"},
        {"§4", R"(</span><span style="color:#AA0000">)"},
        {"§5", R"(</span><span style="color:#AA00AA">)"},
        {"§6", R"(</span><span style="color:#FFAA00">)"},
        {"§7", R"(</span><span style="color:#AAAAAA">)"},
        {"§8", R"(</span><span style="color:#555555">)"},
        {"§9", R"(</span><span style="color:#5555FF">)"},
        {"§a", R"(</span><span style="color:#55FF55">)"},
        {"§b", R"(</span><span style="color:#55FFFF">)"},
        {"§c", R"(</span><span style="color:#FF5555">)"},
        {"§d", R"(</span><span style="color:#FF55FF">)"},
        {"§e", R"(</span><span style="color:#FFFF55">)"},
        {"§f", R"(</span><span style="color:#FFFFFF">)"},
        {"§g", R"(</span><span style="color:#DDD605">)"},
        {"§r", R"(</span>)"}
    };
    for(auto &&[str1, str2] : replaceList)
        str.replace(str1, str2);
    if(auto index = str.indexOf("</span>"); index >= 0 && index < str.indexOf("<span"))
        str.remove(index, 7); //length of </span>
    if(auto index = str.lastIndexOf("</span>"); index < str.lastIndexOf("<span"))
        str.append("</span>");
    return str;
}

QString clearFormat(QString str)
{
    return str.remove(QRegularExpression("<.*?>"));
}

QString timesTo(const QDateTime &dateTime)
{
    auto currrentTime = QDateTime::currentDateTime();
    if(auto seconds = dateTime.secsTo(currrentTime); seconds < 60)
        return QObject::tr("%1 seconds").arg(seconds);
    else if(auto minutes = seconds / 60; minutes < 60)
        return QObject::tr("%1 minutes").arg(minutes);
    else if(auto hours = minutes / 60; hours < 24)
        return QObject::tr("%1 hours").arg(hours);

    if(auto days = dateTime.daysTo(currrentTime); days < 30)
        return QObject::tr("%1 days").arg(days);
    if(auto years = currrentTime.date().year() - dateTime.date().year())
        return QObject::tr("%1 years").arg(years);
    if(auto months = currrentTime.date().month() - dateTime.date().month())
        return QObject::tr("%1 months").arg(months);
    return "";
}

std::function<void ()> disconnecter(QMetaObject::Connection conn){
    return [=]{
        QObject::disconnect(conn);
    };
}
