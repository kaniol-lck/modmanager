#include "knownfile.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

#include "util/tutil.hpp"

KnownFile::KnownFile()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QDir().mkpath(dir.path());
    file_.setFileName(dir.absoluteFilePath(kFileName));
    readFromFile();
}

void KnownFile::addCurseforge(const QString &murmurhash, int curseforgeFileId)
{
    if(murmurhash.isEmpty() || curseforgeFileId == 0) return;
    files()->curseforgeFiles_[murmurhash] = curseforgeFileId;
    files()->writeToFile();
}

void KnownFile::addModrinth(const QString &sha1, QString modrinthFileId)
{
    if(sha1.isEmpty() || modrinthFileId.isEmpty()) return;
    files()->modrinthFiles_[sha1] = modrinthFileId;
    files()->writeToFile();
}

const QMap<QString, int> &KnownFile::curseforgeFiles() const
{
    return curseforgeFiles_;
}

const QMap<QString, QString> &KnownFile::modrinthFiles() const
{
    return modrinthFiles_;
}

KnownFile *KnownFile::files()
{
    static KnownFile files;
    return &files;
}

void KnownFile::writeToFile()
{
    QJsonObject object;
    QJsonObject curseforgeObject;
    for(auto it = curseforgeFiles_.cbegin(); it != curseforgeFiles_.cend(); it++)
        curseforgeObject.insert(it.key(), it.value());
    object.insert("curseforge", curseforgeObject);
    QJsonObject modrinthObject;
    for(auto it = modrinthFiles_.cbegin(); it != modrinthFiles_.cend(); it++)
        modrinthObject.insert(it.key(), it.value());
    object.insert("modrinth", modrinthObject);
    QJsonDocument doc(object);
    if(!file_.open(QIODevice::WriteOnly)) return;
    file_.write(doc.toJson());
    file_.close();
}

void KnownFile::readFromFile()
{
    if(!file_.open(QIODevice::ReadOnly)) return;
    auto bytes = file_.readAll();
    file_.close();

    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return;
    }
    auto result = jsonDocument.toVariant();
    curseforgeFiles_.clear();
    auto &&curseforgeMap = value(result, "curseforge").toMap();
    for(auto it = curseforgeMap.cbegin(); it != curseforgeMap.cend(); it++)
        curseforgeFiles_[it.key()] = it.value().toInt();
    modrinthFiles_.clear();
    auto &&modrinthMap = value(result, "modrinth").toMap();
    for(auto it = modrinthMap.cbegin(); it != modrinthMap.cend(); it++)
        modrinthFiles_[it.key()] = it.value().toString();
}
