#include "forgemodinfo.h"

#include <QDebug>
#include <quazip.h>
#include <quazipfile.h>
#include <toml.hpp>

QList<ForgeModInfo> ForgeModInfo::fromZip(const QString &path)
{
    QuaZip zip(path);
    return fromZip(&zip);
}

QList<ForgeModInfo> ForgeModInfo::fromZip(QuaZip *zip)
{
    QList<ForgeModInfo> list;
    QuaZipFile zipFile(zip);

    //file handles
    //TODO: 1.12-1.13 uses mcmod.info
    if(!zip->open(QuaZip::mdUnzip)) return {};
    zip->setCurrentFile("META-INF/mods.toml");
    if(!zipFile.open(QIODevice::ReadOnly)) return {};
    QByteArray bytes = zipFile.readAll();
    zipFile.close();

    auto config = toml::parse(bytes.data());

    QByteArray iconBytes;
    if(QString iconFilePath = config["logoFile"].value_or(""); !iconFilePath.isEmpty()){
        zip->setCurrentFile(iconFilePath);
        if(zipFile.open(QIODevice::ReadOnly)){
            iconBytes = zipFile.readAll();
            zipFile.close();
        }
    }

    for(auto &i : *config["mods"].as_array()){
        auto t = *i.as_table();

        ForgeModInfo info;
        info.id_ = t["modId"].value_or("");
        info.name_ = t["displayName"].value_or("");
        info.authors_ = QString(t["authors"].value_or("")).split(", ");
        info.description_ = t["description"].value_or("");
        info.iconBytes_ = iconBytes;

        list << info;
    }
    return list;
}
