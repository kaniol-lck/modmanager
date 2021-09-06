#include "localmodinfo.h"

#include <QFile>
#include "quazip.h"
#include "quazipfile.h"
#include <QJsonDocument>
#include <QCryptographicHash>
#include "MurmurHash2.h"
#include <QDebug>
#include <QImage>

#include "util/tutil.hpp"

LocalModInfo::LocalModInfo(QString path) :
    path_(path),
    fileInfo_(path)
{
    acquireInfo(path);
}

const QString &LocalModInfo::id() const
{
    return id_;
}

const QString &LocalModInfo::path() const
{
    return path_;
}

const QString &LocalModInfo::name() const
{
    return name_;
}

const QString &LocalModInfo::version() const
{
    return version_;
}

const QString &LocalModInfo::description() const
{
    return description_;
}

const QString &LocalModInfo::sha1() const
{
    return sha1_;
}

const QString &LocalModInfo::murmurhash() const
{
    return murmurhash_;
}

const QByteArray &LocalModInfo::iconBytes() const
{
    return iconBytes_;
}

const QStringList &LocalModInfo::authors() const
{
    return authors_;
}

bool LocalModInfo::acquireInfo(QString &path)
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
    hasFabricManifest_ = true;

    //collect info
    QVariant result = jsonDocument.toVariant();
    id_ = value(result, "id").toString();
    version_ = value(result, "version").toString();
    name_ = value(result, "name").toString();
    authors_ = value(result, "authors").toStringList();
    description_ = value(result, "description").toString();
    sha1_ = QCryptographicHash::hash(fileContent, QCryptographicHash::Sha1).toHex();

    //icon
    auto iconFilePath = value(result, "icon").toString();
    if(!iconFilePath.isEmpty()){
        modJar.setCurrentFile(iconFilePath);
        if(modJarFile.open(QIODevice::ReadOnly)){
            iconBytes_ = modJarFile.readAll();
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
    murmurhash_ = QByteArray::number(MurmurHash2(filteredFileContent.constData(), filteredFileContent.length(), 1));

    return true;
}

bool LocalModInfo::isFabricMod()
{
    return hasFabricManifest_;
}

QDateTime LocalModInfo::getFileModificationTime() const
{
    return fileInfo_.fileTime(QFile::FileModificationTime);
}
