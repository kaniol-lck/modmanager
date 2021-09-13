#include "funcutil.h"

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
#include <QDir>
#include <QProcess>

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
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isFile())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
        return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args))
        return;
#endif
    qDebug() << info.isFile();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
