#include "bmclapi.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"

const QString BMCLAPI::PREFIX = "https://bmclapi2.bangbang93.com";

BMCLAPI::BMCLAPI(QObject *parent) : QObject(parent)
{}

BMCLAPI *BMCLAPI::api()
{
    static BMCLAPI api;
    return &api;
}

void BMCLAPI::getOptifineList(std::function<void (QList<OptifineModInfo>)> callback)
{
    QUrl url = PREFIX + "/optifine/versionList";

    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }
        auto list = jsonDocument.toVariant().toList();
        QList<OptifineModInfo> optifineList;
        for(const auto &entry : qAsConst(list))
            optifineList << OptifineModInfo::fromVariant(entry);

        callback(optifineList);
        reply->deleteLater();
    });
}

void BMCLAPI::getOptifineDownloadUrl(const OptifineModInfo &info, std::function<void (QUrl)> callback)
{
    QUrl url = PREFIX + QString("/optifine/%1/%2/%3").arg(info.gameVersion(), info.type(), info.patch());
    QNetworkRequest request(url);
#ifndef QT_NO_SSL
    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);
#endif
    static QNetworkAccessManager accessManager;
    auto reply = accessManager.head(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            return;
        }
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull()){
            QUrl downloadUrl = PREFIX + redirection.toString();
            qDebug() << downloadUrl;
            callback(downloadUrl);
        }
        reply->deleteLater();
    });
}
