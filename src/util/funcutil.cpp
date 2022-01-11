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
#include <QStack>

#include "local/localmodpath.h"
#include "local/localfilelinker.h"
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
    QProcess process;
#ifdef Q_OS_UNIX
    QString fileManager;
    process.start("xdg-mime", { "query", "default", "inode/directory" });
    process.waitForFinished();
    fileManager = process.readAllStandardOutput();
    if(fileManager.contains("dolphin")){
        process.startDetached("dolphin", { "--select", filePath });
        return;
    } else if(fileManager.contains("dde-file-manager")){
        process.startDetached("dde-file-manager", { "--show-item", filePath });
        return;
    } else if(fileManager.contains("nautilus")){
        process.startDetached("nautilus", { filePath });
        return;
    } else if(fileManager.contains("nemo")){
        process.startDetached("nemo", { filePath });
        return;
    } else if(fileManager.contains("elementary")){
        process.startDetached("elementary", { filePath });
        return;
    }
#endif
#ifdef Q_OS_WIN
    if(info.isFile()){
        process.startDetached("explorer.exe", { "/select,"+QDir::toNativeSeparators(filePath) });
        return;
    }
#endif
#ifdef Q_OS_MACOS
    process.startDetached("open", { "-R",  filePath });
    return;
#endif
    openFolder(filePath);
}

void openFileInFolder(const QString &fileName, const QString &filePath)
{
    openFileInFolder(QDir(filePath).absoluteFilePath(fileName));
}

void openFolder(const QString &path)
{
    QFileInfo info(path);
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isFile()? info.absolutePath() : path));
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
    for(auto &&localMod : path->modList())
        if(localMod->curseforgeMod() && localMod->curseforgeMod()->modInfo().id() == mod->modInfo().id())
            return true;
    return false;
}

bool hasFile(LocalModPath *path, const CurseforgeFileInfo &fileInfo)
{
    //TODO: old file
    if(hasFile(path->info().path(), fileInfo.fileName())) return true;
    for(auto &&localMod : path->modList())
        if(const auto &fileInfo2 =  localMod->modFile()->linker()->curseforgeFileInfo();
                fileInfo2 && fileInfo2->id() == fileInfo.id())
            return true;
    return false;
}

bool hasFile(LocalModPath *path, ModrinthMod *mod)
{
    for(auto &&localMod : path->modList())
        if(localMod->modrinthMod() && localMod->modrinthMod()->modInfo().id() == mod->modInfo().id())
            return true;
    return false;
}

bool hasFile(LocalModPath *path, const ModrinthFileInfo &fileInfo)
{
    //TODO: old file
    if(hasFile(path->info().path(), fileInfo.fileName())) return true;
    for(auto &&localMod : path->modList())
        if(const auto &fileInfo2 = localMod->modFile()->linker()->modrinthFileInfo();
                fileInfo2 &&fileInfo2->id() == fileInfo.id())
            return true;
    return false;
}

QString capture(const QString &str, const QString &rx, int offset)
{
    QRegularExpression re(rx);
    if(auto match = re.match(str, offset); match.hasMatch())
        return match.captured(1);
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
    QStack<QString> stack;
    static const QMap<QChar, std::tuple<QString, QString>> replaceList{
        { '0', { "</span>", R"(<span style="color:#000000">)" } },
        { '1', { "</span>", R"(<span style="color:#0000AA">)" } },
        { '2', { "</span>", R"(<span style="color:#00AA00">)" } },
        { '3', { "</span>", R"(<span style="color:#00AAAA">)" } },
        { '4', { "</span>", R"(<span style="color:#AA0000">)" } },
        { '5', { "</span>", R"(<span style="color:#AA00AA">)" } },
        { '6', { "</span>", R"(<span style="color:#FFAA00">)" } },
        { '7', { "</span>", R"(<span style="color:#AAAAAA">)" } },
        { '8', { "</span>", R"(<span style="color:#555555">)" } },
        { '9', { "</span>", R"(<span style="color:#5555FF">)" } },
        { 'a', { "</span>", R"(<span style="color:#55FF55">)" } },
        { 'b', { "</span>", R"(<span style="color:#55FFFF">)" } },
        { 'c', { "</span>", R"(<span style="color:#FF5555">)" } },
        { 'd', { "</span>", R"(<span style="color:#FF55FF">)" } },
        { 'e', { "</span>", R"(<span style="color:#FFFF55">)" } },
        { 'f', { "</span>", R"(<span style="color:#FFFFFF">)" } },
        { 'g', { "</span>", R"(<span style="color:#DDD605">)" } },
//        { 'k', { "</span>", R"(<span">)" } },
        { 'l', { "</b>", R"(<b>)" } },
        { 'm', { "</s>", R"(<s>)" } },
        { 'n', { "</u>", R"(<u>)" } },
        { 'o', { "</i>", R"(<i>)" } },
    };
    QString newString;
    for(int i = 0; i < str.size(); i++){
        if(i + 1 < str.size()){
            if(auto ch = str.at(i); ch != "§"){
                newString += ch;
                continue;
            }
            auto code = str.at(i + 1);
            if(replaceList.contains(code)){
                auto &&[end, start] = replaceList[code];
                        if(stack.contains(end)){
                    stack.removeAll(end);
                    newString += end;
                }
                stack << end;
                newString += start;
                //skip next
                i++;
            } else if(code == 'r'){
                while(!stack.isEmpty())
                    newString += stack.takeLast();
                //skip next
                i++;
            }
        } else{
            newString += str.at(i);
            while(!stack.isEmpty())
                newString += stack.takeLast();
        }
    }
    return newString;
}

QString clearFormat(QString str)
{
    return str.remove(QRegularExpression("<.*?>"));
}

QString timesTo(const QDateTime &dateTime)
{
    auto currrentTime = QDateTime::currentDateTime();
    auto years = currrentTime.date().year() - dateTime.date().year();
    auto months = years * 12 + currrentTime.date().month() - dateTime.date().month();
    if(auto seconds = dateTime.secsTo(currrentTime); seconds < 60)
        return QObject::tr("%1 seconds").arg(seconds);
    else if(auto minutes = seconds / 60; minutes < 60)
        return QObject::tr("%1 minutes").arg(minutes);
    else if(auto hours = minutes / 60; hours < 24)
        return QObject::tr("%1 hours").arg(hours);
    else if(auto days = hours / 24; days < 30)
        return QObject::tr("%1 days").arg(days);
    else if(months < 12)
        return QObject::tr("%1 months").arg(months);
    else
        return QObject::tr("%1 years").arg(years);
}

std::function<void ()> disconnecter(QMetaObject::Connection conn){
    return [=]{
        QObject::disconnect(conn);
    };
}

void tweakWidgetFontPointSize(QWidget *widget, int pointSize)
{
    auto font = widget->font();
    font.setPointSize(pointSize);
    widget->setFont(font);
}

uint32_t filteredMurmurHash2(const QByteArray &bytes)
{
  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  QByteArray filteredBytes;
  for (const char& b : qAsConst(bytes)){
      if (b == 0x9 || b == 0xa || b == 0xd || b == 0x20) continue;
      filteredBytes.append(b);
  }

  auto len = filteredBytes.length();
  auto data = filteredBytes.constData();

  uint32_t h = 1 ^ len;

  // Mix 4 bytes at a time into the hash

  while(len >= 4)
  {
      uint32_t k = *(uint32_t*)data;

      k *= m;
      k ^= k >> r;
      k *= m;

      h *= m;
      h ^= k;

      data += 4;
      len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}
