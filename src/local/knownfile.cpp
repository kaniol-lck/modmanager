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

void KnownFile::addCurseforge(const QString &murmurhash,  const CurseforgeFileInfo &file)
{
    if(murmurhash.isEmpty() || !file.id()) return;
    files()->curseforgeFiles_[murmurhash] = file;
    files()->writeToFile();
}

void KnownFile::addModrinth(const QString &sha1, const ModrinthFileInfo &file)
{
    if(sha1.isEmpty() || file.id().isEmpty()) return;
    files()->modrinthFiles_[sha1] = file;
    files()->writeToFile();
}

void KnownFile::addUnmatchedCurseforge(const QString &murmurhash)
{
    if(murmurhash.isEmpty()) return;
    files()->unmatchedCurseforgeFiles_ << murmurhash;
    files()->writeToFile();
}

void KnownFile::addUnmatchedModrinth(const QString &sha1)
{
    if(sha1.isEmpty()) return;
    files()->unmatchedModrinthFiles_ << sha1;
    files()->writeToFile();
}

const QMap<QString, CurseforgeFileInfo> &KnownFile::curseforgeFiles()
{
    return files()->curseforgeFiles_;
}

const QMap<QString, ModrinthFileInfo> &KnownFile::modrinthFiles()
{
    return files()->modrinthFiles_;
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
        curseforgeObject.insert(it.key(), it->toJsonObject());
    object.insert("curseforge", curseforgeObject);
    QJsonObject modrinthObject;
    for(auto it = modrinthFiles_.cbegin(); it != modrinthFiles_.cend(); it++)
        modrinthObject.insert(it.key(), it->toJsonObject());
    object.insert("modrinth", modrinthObject);
    QJsonArray unmatchedCurseforgeObject;
    for(auto &&file : unmatchedCurseforgeFiles_)
        unmatchedCurseforgeObject << file;
    object.insert("unmatchedCurseforge", unmatchedCurseforgeObject);
    QJsonArray unmatchedModrinthObject;
    for(auto &&file : unmatchedModrinthFiles_)
        unmatchedModrinthObject << file;
    object.insert("unmatchedModrinth", unmatchedModrinthObject);
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
    for(auto it = curseforgeMap.cbegin(); it != curseforgeMap.cend(); it++){
        auto &&file = CurseforgeFileInfo::fromVariant(it.value());
        if(curseforgeFiles_.contains(it.key()) || !file.id()) continue;
        curseforgeFiles_[it.key()] = file;
    }
    modrinthFiles_.clear();
    auto &&modrinthMap = value(result, "modrinth").toMap();
    for(auto it = modrinthMap.cbegin(); it != modrinthMap.cend(); it++){
        auto &&file = ModrinthFileInfo::fromVariant(it.value());
        if(modrinthFiles_.contains(it.key()) || file.id().isEmpty()) continue;
        modrinthFiles_[it.key()] = file;
    }
    unmatchedCurseforgeFiles_ = value(result, "unmatchedCurseforge").toStringList();
    unmatchedModrinthFiles_ = value(result, "unmatchedModrinth").toStringList();
}

const QStringList &KnownFile::unmatchedCurseforgeFiles()
{
    return files()->unmatchedCurseforgeFiles_;
}

const QStringList &KnownFile::unmatchedModrinthFiles()
{
    return files()->unmatchedModrinthFiles_;
}
