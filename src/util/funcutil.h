#ifndef FUNCUTIL_H
#define FUNCUTIL_H

#include <QString>

#include "curseforge/curseforgemodinfo.h"
#include "modrinth/modrinthmodinfo.h"

class QNetworkAccessManager;
class LocalModPath;
class CurseforgeMod;
class ModrinthMod;

QString numberConvert(int size, const QString &suffix, int prec = 3, int limit = 850);

void openFileInFolder(const QString &filePath);

bool hasFile(const QString &path, const QString &fileName);
bool hasFile(LocalModPath *path, CurseforgeMod *mod);
bool hasFile(LocalModPath *path, const CurseforgeFileInfo &modInfo);
bool hasFile(LocalModPath *path, ModrinthMod *mod);
bool hasFile(LocalModPath *path, const ModrinthFileInfo &modInfo);

QString capture(const QString &str, const QString &regExp, bool minimal = true);

#endif // FUNCUTIL_H
