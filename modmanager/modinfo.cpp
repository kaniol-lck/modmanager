#include "modinfo.h"

#include <QFile>
#include "quazip.h"
#include "quazipfile.h"
#include <QJsonDocument>
#include <QCryptographicHash>
#include "MurmurHash2.h"
#include <QDebug>
#include <QImage>

#include "util/qjsonutil.hpp"

ModInfo::ModInfo(QString path) :
    modPath(path),
    modFileInfo(path)
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

const QByteArray &ModInfo::getIconBytes() const
{
    return iconBytes;
}

bool ModInfo::acquireInfo(QString &path)
{
    QFile modFile(path);
    QuaZip modJar(path);
    QuaZipFile modJarFile(&modJar);

    //file open error
    if(!modFile.open(QIODevice::ReadOnly)) return false;
    QByteArray fileContent = modFile.readAll();
    modFile.close();

    //file handles
    if(!modJar.open(QuaZip::mdUnzip)) return false;
    modJar.setCurrentFile("fabric.mod.json");
    if(!modJarFile.open(QIODevice::ReadOnly)) return false;
    QByteArray json = modJarFile.readAll();
    modJarFile.close();

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
    sha1 = QCryptographicHash::hash(fileContent, QCryptographicHash::Sha1).toHex();

    //icon
    auto iconFilePath = value(result, "icon").toString();
    if(!iconFilePath.isEmpty()){
        modJar.setCurrentFile(iconFilePath);
        if(modJarFile.open(QIODevice::ReadOnly)){
            iconBytes = modJarFile.readAll();
            modJarFile.close();
        }
    }
    modJar.close();

    //exclude some bytes for murmurhash
    QByteArray filteredFileContent;
    for (const char& b : qAsConst(fileContent)){
        if (b == 0x9 || b == 0xa || b == 0xd || b == 0x20) continue;
        filteredFileContent.append(b);
    }
    murmurhash = QByteArray::number(MurmurHash2(filteredFileContent.constData(), filteredFileContent.length(), 1));

    return true;
}

bool ModInfo::isFabricMod()
{
    return hasFabricManifest;
}

QDateTime ModInfo::getFileModificationTime() const
{
    return modFileInfo.fileTime(QFile::FileModificationTime);
}
