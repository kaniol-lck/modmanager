#include "gameversion.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QEventLoop>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"

QStringList GameVersion::versionList;

GameVersion::GameVersion(const QString string) :
    versionString(string)
{

}

const QString &GameVersion::getVersionString() const
{
    return versionString;
}

GameVersion::operator QString() const
{
    return versionString;
}

std::optional<GameVersion> GameVersion::deduceFromString(const QString &string)
{
    QRegExp re(R"((\d+\.\d+(\.\d+)?))");
    qDebug() << string;
    qDebug() << re.indexIn(string);
    if(re.indexIn(string)){
        //2nd cap
        auto str = re.cap(1);
        if(versionList.contains(str))
            return {GameVersion(str)};
    }
    return std::nullopt;
}

void GameVersion::initVersionList()
{
    //get version list
    QEventLoop loop;
    QNetworkAccessManager accessManager;
    QNetworkRequest request(QUrl("https://addons-ecs.forgesvc.net/api/v2/minecraft/version"));
    auto reply = accessManager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() == QNetworkReply::NoError){
        QJsonParseError error;
        auto json = QJsonDocument::fromJson(reply->readAll(), &error);
        if(error.error == QJsonParseError::NoError){
            for(const auto &entry : json.toVariant().toList())
                versionList.append(value(entry, "versionString").toString());
        }
    } else{
        //default list
        versionList = QStringList{
            "1.17.1",
            "1.17",
            "1.16.5",
            "1.16.4",
            "1.16.3",
            "1.16.2",
            "1.16.1",
            "1.16",
            "1.15.2",
            "1.15.1",
            "1.15",
            "1.14.4",
            "1.14.3",
            "1.14.2",
            "1.14.1",
            "1.14",
            "1.13.2",
            "1.13.1",
            "1.13",
            "1.12.2",
            "1.12.1",
            "1.12",
            "1.11.2",
            "1.11.1",
            "1.11",
            "1.10.2",
            "1.10.1",
            "1.10",
            "1.9.4",
            "1.9.3",
            "1.9.2",
            "1.9.1",
            "1.9",
            "1.8.9",
            "1.8.8",
            "1.8.7",
            "1.8.6",
            "1.8.5",
            "1.8.4",
            "1.8.3",
            "1.8.2",
            "1.8.1",
            "1.8",
            "1.7.10",
            "1.7.9",
            "1.7.8",
            "1.7.7",
            "1.7.6",
            "1.7.5",
            "1.7.4",
            "1.7.3",
            "1.7.2",
            "1.6.4",
            "1.6.2",
            "1.6.1",
            "1.5.2",
            "1.5.1",
            "1.4.7",
            "1.4.6",
            "1.4.5",
            "1.4.4",
            "1.4.2",
            "1.3.2",
            "1.3.1",
            "1.2.5",
            "1.2.4",
            "1.2.3",
            "1.2.2",
            "1.2.1",
            "1.1",
            "1.0"
        };
    }
}
