#include "optifineapi.h"

#include <QNetworkReply>
#include <QUrlQuery>

#include "util/funcutil.h"

OptifineAPI::OptifineAPI(QObject *parent) : QObject(parent)
{}

OptifineAPI *OptifineAPI::api()
{
    static OptifineAPI api;
    return &api;
}

void OptifineAPI::getModList(std::function<void (QList<OptifineModInfo>)> callback)
{
    QUrl url("https://optifine.net/downloads");
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return;
        }
        auto webPage = reply->readAll();
        reply->deleteLater();

        QRegExp re(R"(<tr class='downloadLine.*'>(.*)</tr>)");
        re.setMinimal(true);
        int pos = 0;
        QList<OptifineModInfo> list;
        GameVersion gameVersion = capture(webPage, "<h2>Minecraft (.+)</h2>", true, pos);
        GameVersion nextGameVersion;
        while ((pos = re.indexIn(webPage, pos)) != -1) {
            if(auto gv = capture(webPage, "<h2>Minecraft (.+)</h2>", true, pos); gv != nextGameVersion){
                if(nextGameVersion != GameVersion::Any) gameVersion = nextGameVersion;
                nextGameVersion = gv;
            }
            list << OptifineModInfo::fromHtml(re.cap(1), gameVersion);
            pos +=  re.matchedLength();
        }
        callback(list);
    });
}

void OptifineAPI::getDownloadUrl(const QString &fileName, std::function<void (QUrl)> callback)
{
    QUrl url("https://optifine.net/adloadx");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("f", fileName);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return;
        }
        auto webPage = reply->readAll();
        reply->deleteLater();
        auto str = capture(webPage, R"(<a href='(.*)' onclick='.*'>.*</a>)");
        QUrl downloadUrl = "https://optifine.net/" + str;
        callback(downloadUrl);
    });
}
