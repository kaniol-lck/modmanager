#include "localmodinfo.h"

#include <QFile>
#include "quazip.h"
#include "quazipfile.h"
#include <QJsonDocument>
#include <QCryptographicHash>
#include "MurmurHash2.h"
#include <QDebug>
#include <QImage>
#include <QBuffer>

#include "util/tutil.hpp"

LocalModInfo::LocalModInfo(QString path) :
    path_(path),
    fileInfo_(path)
{
    QFile modFile(path);
    QuaZip zip(path);

    //file open error
    if(!modFile.open(QIODevice::ReadOnly)) return;
    QByteArray fileContent = modFile.readAll();
    modFile.close();

    //sha1
    sha1_ = QCryptographicHash::hash(fileContent, QCryptographicHash::Sha1).toHex();

    //exclude some bytes for murmurhash
    QByteArray filteredFileContent;
    for (const char& b : qAsConst(fileContent)){
        if (b == 0x9 || b == 0xa || b == 0xd || b == 0x20) continue;
        filteredFileContent.append(b);
    }
    murmurhash_ = QByteArray::number(MurmurHash2(filteredFileContent.constData(), filteredFileContent.length(), 1));

    //load fabric mod
    fabricModInfoList_ = FabricModInfo::fromZip(&zip);
    if(!fabricModInfoList_.isEmpty()){
        loaderType_ = ModLoaderType::Fabric;
        fabricModInfoList_.first().setIsEmbedded(false);
    }
}

bool LocalModInfo::operator==(const LocalModInfo &other) const
{
    //same path
    return path_ == other.path_;
}

void LocalModInfo::addOld()
{
    path_.append(".old");
}

void LocalModInfo::removeOld()
{
    path_.remove(".old");
}

const QString &LocalModInfo::path() const
{
    return path_;
}

const QFileInfo &LocalModInfo::fileInfo() const
{
    return fileInfo_;
}

const QString &LocalModInfo::sha1() const
{
    return sha1_;
}

const QString &LocalModInfo::murmurhash() const
{
    return murmurhash_;
}

ModLoaderType::Type LocalModInfo::loaderType() const
{
    return loaderType_;
}

FabricModInfo LocalModInfo::fabric() const
{
    return fabricModInfoList_.first();
}

const QList<FabricModInfo> &LocalModInfo::fabricModInfoList() const
{
    return fabricModInfoList_;
}
