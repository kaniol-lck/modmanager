#include "youdaotranslator.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QStandardPaths>

#include "util/tutil.hpp"

YoudaoTranslator::YoudaoTranslator(QObject *parent) :
    QObject(parent)
{
    diskCache_.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    accessManager_.setCache(&diskCache_);
}

YoudaoTranslator *YoudaoTranslator::translator()
{
    static YoudaoTranslator translator;
    return &translator;
}

void YoudaoTranslator::translate(const QString &str, std::function<void (QString)> callback)
{
    QUrl url = "http://fanyi.youdao.com/translate?&doctype=json&type=EN2ZH_CN&i=" + str;

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
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
        auto result = jsonDocument.toVariant();
        QString translated;
        for(auto &&entry : value(result, "translateResult").toList().first().toList())
            translated += value(entry, "tgt").toString();
        callback(translated);
        reply->deleteLater();
    });
}
