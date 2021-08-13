#include "modinfo.h"

#include <QFile>
#include "quazip.h"
#include "quazipfile.h"
#include <QJsonDocument>
#include <QCryptographicHash>
#include "MurmurHash2.h"

#include "util/qjsonutil.hpp"

ModInfo::ModInfo(QString path) :
    modPath(path)
{
    acquireInfo(path);
}

const QString &ModInfo::getId() const
{
    return id;
}

const QString &ModInfo::getModPath() const
{
    return modPath;
}

const QString &ModInfo::getName() const
{
    return name;
}

const QString &ModInfo::getVersion() const
{
    return version;
}

const QString &ModInfo::getDescription() const
{
    return description;
}

const QString &ModInfo::getSha1() const
{
    return sha1;
}

const QString &ModInfo::getMurmurhash() const
{
    return murmurhash;
}

bool ModInfo::acquireInfo(QString &path)
{
    QFile modFile(path);
    QuaZip modJar(path);
    QuaZipFile modJarFile(&modJar);

    //file open error
    if(!modFile.open(QIODevice::ReadOnly)) return false;
    QByteArray jarContent = modFile.readAll();
    modFile.close();

    //jar file and fabric mod json
    if(!modJar.open(QuaZip::mdUnzip)) return false;
    modJar.setCurrentFile("fabric.mod.json");
    if(!modJarFile.open(QIODevice::ReadOnly)) return false;
    QByteArray json = modJarFile.readAll();
    modJarFile.close();
    modJar.close();

    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return false;
    }

    if(!jsonDocument.isObject()) return false;

    //all pass
    hasFabricManifest = true;

    //collect info
    QVariant result = jsonDocument.toVariant();
    id = value(result, "id").toString();
    version = value(result, "version").toString();
    name = value(result, "name").toString();
    description = value(result, "description").toString();
    sha1 = QCryptographicHash::hash(jarContent, QCryptographicHash::Sha1).toHex();
    murmurhash = QByteArray::number(MurmurHash2(jarContent.data(), jarContent.length(), 1));

    return true;
}

bool ModInfo::isFabricMod()
{
    return hasFabricManifest;
}
