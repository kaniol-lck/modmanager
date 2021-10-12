#include "idmapper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

#include "util/tutil.hpp"

IdMapper::IdMapper()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QDir().mkpath(dir.path());
    file_.setFileName(dir.absoluteFilePath(kFileName));
    readFromFile();
}

IdMapper *IdMapper::mapper()
{
    static IdMapper mapper;
    return &mapper;
}

const QMap<QString, IdMapper::Id> &IdMapper::idMap()
{
    return mapper()->idMap_;
}

IdMapper::Id IdMapper::get(const QString &modid)
{
    return mapper()->idMap_[modid];
}

void IdMapper::addCurseforge(const QString &modid, int curseforgeId)
{
    if(modid.isEmpty())
        qDebug() << curseforgeId;
    if(!mapper()->idMap_.contains(modid))
        mapper()->idMap_.insert(modid, Id(modid));
    mapper()->idMap_[modid].curseforgeId_ = curseforgeId;
    mapper()->writeToFile();
}

void IdMapper::addModrinth(const QString &modid, QString modrinthId)
{
    if(modid.isEmpty())
        qDebug() << modrinthId;
    if(!mapper()->idMap_.contains(modid))
        mapper()->idMap_.insert(modid, Id(modid));
    mapper()->idMap_[modid].modrinthId_ = modrinthId;
    mapper()->writeToFile();
}

void IdMapper::writeToFile()
{
    QJsonObject object;
    for(const auto &id : qAsConst(idMap_))
        object.insert(id.modid(), id.toJsonObject());
    QJsonDocument doc(object);
    if(!file_.open(QIODevice::WriteOnly)) return;
    file_.write(doc.toJson());
    file_.close();
}

void IdMapper::readFromFile()
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
    auto result = jsonDocument.toVariant().toMap();
    idMap_.clear();
    for(auto it = result.cbegin(); it != result.cend(); it++)
        idMap_[it.key()] = Id::fromJsonObject(it.key(), *it);
}

IdMapper::Id::Id(const QString &modid) :
    modid_(modid)
{}

IdMapper::Id IdMapper::Id::fromJsonObject(const QString &modid, const QVariant &variant)
{
    Id id(modid);
    id.curseforgeId_ = value(variant, "curseforge").toInt();
    id.modrinthId_ = value(variant, "modrinth").toString();
    return id;
}

QJsonObject IdMapper::Id::toJsonObject() const
{
    QJsonObject object;
    if(curseforgeId_)
        object.insert("curseforge", curseforgeId_);
    if(!modrinthId_.isEmpty())
        object.insert("modrinth", modrinthId_);
    return object;
}

const QString &IdMapper::Id::modid() const
{
    return modid_;
}

int IdMapper::Id::curseforgeId() const
{
    return curseforgeId_;
}

const QString &IdMapper::Id::modrinthId() const
{
    return modrinthId_;
}
