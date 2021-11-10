#ifndef FUNCUTIL_H
#define FUNCUTIL_H

#include <QString>

#include "curseforge/curseforgemodinfo.h"
#include "modrinth/modrinthmodinfo.h"

class QNetworkAccessManager;
class LocalModPath;
class CurseforgeMod;
class ModrinthMod;

QString sizeConvert(qint64 byte, int prec = 3, int limit = 900);
QString speedConvert(qint64 byte, int prec = 3, int limit = 900);
QString numberConvert(int size, const QString &suffix, int prec = 3, int limit = 900);

void openFileInFolder(const QString &filePath);

bool hasFile(const QString &path, const QString &fileName);
bool hasFile(LocalModPath *path, CurseforgeMod *mod);
bool hasFile(LocalModPath *path, const CurseforgeFileInfo &fileInfo);
bool hasFile(LocalModPath *path, ModrinthMod *mod);
bool hasFile(LocalModPath *path, const ModrinthFileInfo &fileInfo);

QString capture(const QString &str, const QString &regExp, bool minimal = true, int offset = 0);

QStringList getExistingDirectories(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString());

QString colorCodeFormat(QString str);

QString clearFormat(QString str);

QString timesTo(const QDateTime &dateTime);

std::function<void ()> disconnecter(QMetaObject::Connection conn);;

#endif // FUNCUTIL_H
