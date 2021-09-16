#include "localmodfileinfo.h"

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

QString LocalModFileInfo::id() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().id();
    else
        ;//todo
}

QString LocalModFileInfo::name() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().name();
    else
        ;//todo
}

QString LocalModFileInfo::version() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().version();
    else
        ;//todo
}

QString LocalModFileInfo::description() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().description();
    else
        ;//todo
}

QByteArray LocalModFileInfo::iconBytes() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().iconBytes();
    else
        ;//todo
}

QStringList LocalModFileInfo::authors() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().authors();
    else
        ;//todo
}

QUrl LocalModFileInfo::website() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().homepage();
    else
        ;//todo
}

QUrl LocalModFileInfo::sources() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().sources();
    else
        ;//todo
}

QUrl LocalModFileInfo::issues() const
{
    if(loaderType_ == ModLoaderType::Fabric)
        return fabric().issues();
    else
        ;//todo
}

ModLoaderType::Type LocalModFileInfo::loaderType() const
{
    return loaderType_;
}

FabricModInfo LocalModFileInfo::fabric() const
{
    return fabricModInfoList_.first();
}

const QList<FabricModInfo> &LocalModFileInfo::fabricModInfoList() const
{
    return fabricModInfoList_;
}
