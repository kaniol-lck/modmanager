#ifndef KNOWNFILE_H
#define KNOWNFILE_H

#include <QFile>
#include <QMap>
#include <QString>

class KnownFile
{
public:
    KnownFile();
    static void addCurseforge(const QString &murmurhash, int curseforgeFileId);
    static void addModrinth(const QString &sha1, QString modrinthFileId);
    const QMap<QString, int> &curseforgeFiles() const;
    const QMap<QString, QString> &modrinthFiles() const;

private:
    static KnownFile *files();
    void writeToFile();
    void readFromFile();

    static constexpr auto kFileName = "fileid.json";
    QFile file_;
    QMap<QString, int> curseforgeFiles_;
    QMap<QString, QString> modrinthFiles_;
};

#endif // KNOWNFILE_H
