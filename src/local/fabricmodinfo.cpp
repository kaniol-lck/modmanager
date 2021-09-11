#include "fabricmodinfo.h"

#include <QJsonDocument>
#include <QCryptographicHash>
#include <QBuffer>
#include "quazip.h"
#include "quazipfile.h"

#include "util/tutil.hpp"

QList<FabricModInfo> FabricModInfo::fromZip(QuaZip *zip)
{
    QList<FabricModInfo> list;
    QuaZipFile zipFile(zip);

    //file handles
    if(!zip->open(QuaZip::mdUnzip)) return {};
    zip->setCurrentFile("fabric.mod.json");
    if(!zipFile.open(QIODevice::ReadOnly)) return {};
    QByteArray json = zipFile.readAll();
    zipFile.close();

    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return {};
    }

    if(!jsonDocument.isObject()) return {};

    //all pass
    FabricModInfo info;

    //collect info
    QVariant result = jsonDocument.toVariant();
    info.id_ = value(result, "id").toString();
    info.version_ = value(result, "version").toString();
    info.name_ = value(result, "name").toString();
    info.authors_ = value(result, "authors").toStringList();
    info.description_ = value(result, "description").toString();

    //provides
    info.provides_ = value(result, "provides").toStringList();

    //depends
    auto dependsMap = value(result, "depends").toMap();
    for(auto it = dependsMap.cbegin(); it != dependsMap.cend(); it++)
        info.depends_[it.key()] = it.value().toString();

    //conflicts
    auto conflictsMap = value(result, "conflicts").toMap();
    for(auto it = conflictsMap.cbegin(); it != conflictsMap.cend(); it++)
        info.conflicts_[it.key()] = it.value().toString();

    //breaks
    auto breaksMap = value(result, "breaks").toMap();
    for(auto it = breaksMap.cbegin(); it != breaksMap.cend(); it++)
        info.breaks_[it.key()] = it.value().toString();

    //icon
    auto iconFilePath = value(result, "icon").toString();
    if(!iconFilePath.isEmpty()){
        zip->setCurrentFile(iconFilePath);
        if(zipFile.open(QIODevice::ReadOnly)){
            info.iconBytes_ = zipFile.readAll();
            zipFile.close();
        }
    }

    //append info
    list << info;

    //embedded jars
    auto jarList = value(result, "jars").toList();
    for(const auto &jar : jarList){
        auto jarPath = value(jar, "file").toString();
        zip->setCurrentFile(jarPath);
        if(zipFile.open(QIODevice::ReadOnly)){
            auto embededJarBytes = zipFile.readAll();
            zipFile.close();
            QBuffer buffer(&embededJarBytes);
            QuaZip embeddedZip(&buffer);

            //append embedded info
            list << fromZip(&embeddedZip);
        }
    }
    zip->close();

    return list;
}

const QString &FabricModInfo::id() const
{
    return id_;
}

const QString &FabricModInfo::name() const
{
    return name_;
}

const QString &FabricModInfo::version() const
{
    return version_;
}

const QStringList &FabricModInfo::authors() const
{
    return authors_;
}

const QString &FabricModInfo::description() const
{
    return description_;
}

const QByteArray &FabricModInfo::iconBytes() const
{
    return iconBytes_;
}

const QStringList &FabricModInfo::provides() const
{
    return provides_;
}

const QMap<QString, QString> &FabricModInfo::depends() const
{
    return depends_;
}

const QMap<QString, QString> &FabricModInfo::conflicts() const
{
    return conflicts_;
}

const QMap<QString, QString> &FabricModInfo::breaks() const
{
    return breaks_;
}

bool FabricModInfo::isEmbedded() const
{
    return isEmbedded_;
}

void FabricModInfo::setIsEmbedded(bool newIsEmbedded)
{
    isEmbedded_ = newIsEmbedded;
}
