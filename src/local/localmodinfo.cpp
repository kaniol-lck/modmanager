#include "localmodinfo.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
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
    fabricModInfoList_ = FabricModInfo::fromZip(path);
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
    fileInfo_.setFile(path_);
}

void LocalModInfo::removeOld()
{
    path_.remove(".old");
    fileInfo_.setFile(path_);
}

QString LocalModInfo::id() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().id();
    else
        ;//todo
}

QString LocalModInfo::name() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().name();
    else
        ;//todo
}

QString LocalModInfo::version() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().version();
    else
        ;//todo
}

QString LocalModInfo::description() const
{

    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().description();
    else
        ;//todo
}

QByteArray LocalModInfo::iconBytes() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().iconBytes();
    else
        ;//todo
}

QStringList LocalModInfo::authors() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().authors();
    else
        ;//todo
}

QUrl LocalModInfo::website() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().homepage();
    else
        ;//todo
}

QUrl LocalModInfo::sources() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().sources();
    else
        ;//todo
}

QUrl LocalModInfo::issues() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().issues();
    else
        ;//todo
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

std::tuple<QString, QString> LocalModInfo::baseNameFullSuffix() const
{
    auto fileName = fileInfo_.fileName();
    QFileInfo fileInfo(fileName);
    QString fullSuffix;
    while(true){
        fileInfo.setFile(fileName);
        auto suffix = fileInfo.suffix();
        if(suffix != "jar" && suffix != "old")
            break;
        fileName = fileInfo.completeBaseName();
        fullSuffix += "." + suffix;
    }
    return { fileName, fullSuffix };
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
