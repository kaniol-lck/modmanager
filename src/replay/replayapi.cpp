#include "replayapi.h"

#include <QNetworkReply>
#include <QUrlQuery>

#include "util/funcutil.h"
#include "config.hpp"

const QString ReplayAPI::PREFIX = "https://www.replaymod.com";

ReplayAPI::ReplayAPI(QObject *parent) : QObject(parent)
{
    accessManager_.setTransferTimeout(Config().getNetworkRequestTimeout());
}

ReplayAPI *ReplayAPI::api()
{
    static ReplayAPI api;
    return &api;
}

void ReplayAPI::getModList(std::function<void (QList<ReplayModInfo>)> callback)
{
    QUrl url = PREFIX + "/download/";
    QNetworkRequest request(url);
    auto reply = accessManager_.get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return;
        }
        auto webPage = reply->readAll();
        reply->deleteLater();
        QRegExp re(R"((<th>Replay Mod .*)</tr>)");
        re.setMinimal(true);
        int pos = webPage.indexOf("All Versions");
        QList<ReplayModInfo> list;
        GameVersion gameVersion = capture(webPage, "<h3>(Minecraft .+) ?.*</h3>", true, pos);
        GameVersion nextGameVersion;
        while ((pos = re.indexIn(webPage, pos)) != -1) {
            if(auto gv = capture(webPage, "<h3>(Minecraft .+) ?.*</h3>", true, pos); gv != nextGameVersion){
                if(nextGameVersion != GameVersion::Any) gameVersion = nextGameVersion;
                nextGameVersion = gv;
            }
            list << ReplayModInfo::fromHtml(re.cap(1), gameVersion);
            pos +=  re.matchedLength();
        }
        callback(list);
    });
}
