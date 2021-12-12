#ifndef KNOWNFILE_H
#define KNOWNFILE_H

#include <QFile>
#include <QMap>
#include <QString>

#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"

class KnownFile
{
public:
    KnownFile();
    static void addCurseforge(const QString &murmurhash, const CurseforgeFileInfo &file);
    static void addModrinth(const QString &sha1, const ModrinthFileInfo &file);
    static void addUnmatchedCurseforge(const QString &murmurhash);
    static void addUnmatchedModrinth(const QString &sha1);

    static void clearUnmatched();

    static const QMap<QString, CurseforgeFileInfo> &curseforgeFiles();
    static const QMap<QString, ModrinthFileInfo> &modrinthFiles();
    static const QStringList &unmatchedCurseforgeFiles();
    static const QStringList &unmatchedModrinthFiles();

private:
    static KnownFile *files();
    void writeToFile();
    void readFromFile();

    static constexpr auto kFileName = "fileid.json";
    QFile file_;
    QMap<QString, CurseforgeFileInfo> curseforgeFiles_;
    QMap<QString, ModrinthFileInfo> modrinthFiles_;
    QStringList unmatchedCurseforgeFiles_;
    QStringList unmatchedModrinthFiles_;
};

#endif // KNOWNFILE_H
