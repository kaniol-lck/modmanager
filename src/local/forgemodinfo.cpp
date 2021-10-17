#include "forgemodinfo.h"

#include <QDebug>
#include <quazip.h>
#include <quazipfile.h>
#include <QJsonDocument>
#include <QRegularExpression>
#include <toml.hpp>

#include "util/funcutil.h"
#include "util/tutil.hpp"

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

    //modern forge
    zip->setCurrentFile("META-INF/mods.toml");
    if(zipFile.open(QIODevice::ReadOnly)){
        QByteArray bytes = zipFile.readAll();
        zipFile.close();

        QString string = bytes;

        auto i = QRegularExpression(R"("[^"]*(?:""[^"]*)*")").globalMatch(bytes);
        while (i.hasNext()){
            QRegularExpressionMatch match = i.next();
            if(!match.captured().contains('\n')) continue;
            auto str = match.captured().replace('\n', "\\n");
            string.replace(match.captured(), str);
        }

        toml::parse_result config;
        try {
            config = toml::parse(string.toUtf8().data());
        }  catch (...) {
            qDebug() << zip->getZipName() << "has invalid toml file.";
            return {};
        }

        //icon
        auto icon = [&](const auto &variant){
            if(QString iconFilePath = variant["logoFile"].value_or(""); !iconFilePath.isEmpty()){
                zip->setCurrentFile(iconFilePath);
                if(zipFile.open(QIODevice::ReadOnly)){
                    auto iconBytes = zipFile.readAll();
                    zipFile.close();
                    return iconBytes;
                }
            }
            return QByteArray();
        };

        QUrl issue(config["issueTrackerURL"].value_or(""));

        auto split = [=](QString str){
            QStringList list;
            if(str.isEmpty())
                return list;
            else
                for(auto &&author : str.split(", "))
                    list << colorCodeFormat(author);
            return list;
        };

        QStringList authors = split(config["authors"].value_or(""));

        for(auto &i : *config["mods"].as_array()){
            auto modTable = *i.as_table();

            ForgeModInfo info;
            info.id_ = modTable["modId"].value_or("");
            info.name_ = colorCodeFormat(modTable["displayName"].value_or(""));
            info.version_ = modTable["version"].value_or("");
            info.authors_ << authors << split(modTable["authors"].value_or(""));
            info.description_ = colorCodeFormat(modTable["description"].value_or(""));
            info.iconBytes_ = icon(modTable);
            if(info.iconBytes_.isEmpty())
                info.iconBytes_ = icon(config);
            info.credits_ = modTable["credits"].value_or("");
            info.updateUrl_ = modTable["updateJSONURL"].value_or("");

            info.issues_ = issue;
            info.homepage_ = modTable["displayURL"].value_or("");

            if(info.version_ == "${file.jarVersion}")
                info.version_ = jarVersion;

            list << info;
        }
    }

    //legacy forge
    zip->setCurrentFile("mcmod.info");
    if(zipFile.open(QIODevice::ReadOnly)){
        QByteArray bytes = zipFile.readAll();
        zipFile.close();

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return {};
        }

        if(!jsonDocument.isArray()) return {};

        for(auto &&result : jsonDocument.toVariant().toList()){
            ForgeModInfo info;
            info.id_ = value(result, "modid").toString();
            info.name_ = value(result, "name").toString();
            info.version_ = value(result, "version").toString();
            info.authors_ = value(result, "authors").toStringList();
            info.description_ = value(result, "description").toString();
            info.homepage_ = value(result, "url").toUrl();
            info.credits_ = value(result, "credits").toString();
            info.updateUrl_ = value(result, "updateUrl").toUrl();

            //icon
            if(auto iconFilePath = value(result, "logoFile").toString(); !iconFilePath.isEmpty()){
                zip->setCurrentFile(iconFilePath);
                if(zipFile.open(QIODevice::ReadOnly)){
                    info.iconBytes_ = zipFile.readAll();
                    zipFile.close();
                }
            }

            list << info;
        }
    }

    return list;
}
