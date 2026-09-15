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
        for(const auto &entry : qAsConst(list)){
            auto info = OptifineModInfo::fromVariant(entry);
            // versionList 里已经带了 filename，直链可以就地拼出来：
            //     <PREFIX>/maven/com/optifine/<mcversion>/<filename>
            // （实测这正是 /optifine/<mc>/<type>/<patch> 的 302 目标，可直接下载）
            // 这样 497 个条目一个额外请求都不用发，也不依赖重定向解析——
            // 原实现是每个条目单独发一次 HEAD 去读重定向，且读法本身是错的（见 getOptifineDownloadUrl）。
            if(!info.fileName().isEmpty()){
                const QUrl url(PREFIX + QString("/maven/com/optifine/%1/%2").arg(info.gameVersion(), info.fileName()));
                info.setMirrorUrl(url);
                info.setDownloadUrl(url);
            }
            optifineList << info;
        }

        return optifineList;
    } };
}

Reply<QUrl> BMCLAPI::getOptifineDownloadUrl(const OptifineModInfo &info)
{
    QUrl url = PREFIX + QString("/optifine/%1/%2/%3").arg(info.gameVersion(), info.type(), info.patch());
    QNetworkRequest request(url);
    // 必须显式改为手动跟随重定向。QNetworkRequest 的默认策略是 NoLessSafeRedirectPolicy
    // （自动跟随）：此时 reply 上挂着的是最终 200 的响应，RedirectionTargetAttribute 为
    // **null**（Qt 6 实测：默认策略 isNull=true；ManualRedirectPolicy 下才是 302 + Location）。
    // 原实现按默认策略去读这个属性，于是这里总是返回空 QUrl ——
    // 表现就是 OptiFine 列表能列出来、下载按钮却一直灰着，用户看到"只有版本列表、没有下载链接"。
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
#ifndef QT_NO_SSL
    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);
#endif
    auto reply = accessManager_.head(request);
    return { reply, [=]{
        if(auto redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute); !redirection.isNull()){
            QUrl downloadUrl = PREFIX + redirection.toUrl().toString();
            // Location 里带的就是真实文件名，可作为 filename 字段缺失时的兜底
            return downloadUrl;
        }
        return QUrl{};
    } };
}
