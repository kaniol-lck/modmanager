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
    if(!zip->open(QuaZip::mdUnzip)) return {};

    //${file.jarVersion}
    zip->setCurrentFile("META-INF/MANIFEST.MF");
    if(!zipFile.open(QIODevice::ReadOnly)) return {};
    QString manifest = zipFile.readAll();
    zipFile.close();

    QString jarVersion;
    QRegExp re(R"(Implementation-Version:\s?([^\s]+))");
    if(auto i = re.indexIn(manifest); i)
        jarVersion = re.cap(1);

    //TODO: 1.12-1.13 uses mcmod.info
    zip->setCurrentFile("META-INF/mods.toml");
    if(!zipFile.open(QIODevice::ReadOnly)) return {};
    QByteArray bytes = zipFile.readAll();
    zipFile.close();

    toml::parse_result config;
    try {
        config = toml::parse(bytes.data());
    }  catch (...) {
        qDebug() << zip->getZipName() << "has invalid toml file.";
        return {};
    }

    //icon
    QByteArray iconBytes;
    if(QString iconFilePath = config["logoFile"].value_or(""); !iconFilePath.isEmpty()){
        zip->setCurrentFile(iconFilePath);
        if(zipFile.open(QIODevice::ReadOnly)){
            iconBytes = zipFile.readAll();
            zipFile.close();
        }
    }

    QUrl issue(config["issueTrackerURL"].value_or(""));

    auto split = [=](QString str){
        if(str.isEmpty())
            return QStringList{};
        else
            return str.split(", ");
    };

    QStringList authors = split(config["authors"].value_or(""));

    for(auto &i : *config["mods"].as_array()){
        auto t = *i.as_table();

        ForgeModInfo info;
        info.id_ = t["modId"].value_or("");
        info.name_ = t["displayName"].value_or("");
        info.version_ = t["version"].value_or("");
        info.authors_ << authors << split(t["authors"].value_or(""));
        info.description_ = t["description"].value_or("");
        info.iconBytes_ = iconBytes;

        info.issues_ = issue;
        info.homepage_ = t["displayURL"].value_or("");

        if(info.version_ == "${file.jarVersion}")
            info.version_ = jarVersion;

        list << info;
    }
    return list;
}
