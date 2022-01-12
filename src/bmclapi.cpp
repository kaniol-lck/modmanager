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

Reply<QList<OptifineModInfo> > BMCLAPI::getOptifineList()
{
    QUrl url = PREFIX + "/optifine/versionList";

    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<OptifineModInfo>{};
        }
        auto list = jsonDocument.toVariant().toList();
        QList<OptifineModInfo> optifineList;
        for(const auto &entry : qAsConst(list))
            optifineList << OptifineModInfo::fromVariant(entry);

        return optifineList;
    } };
}

Reply<QUrl> BMCLAPI::getOptifineDownloadUrl(const OptifineModInfo &info)
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
    return { reply, [=]{
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull()){
            QUrl downloadUrl = PREFIX + redirection.toString();
            return downloadUrl;
        }
        return QUrl{};
    } };
}
