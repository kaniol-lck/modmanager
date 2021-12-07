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
        QRegularExpression re(R"((<th>Replay Mod .*?)</tr>)");
        int pos = webPage.indexOf("All Versions");
        QList<ReplayModInfo> list;
        GameVersion gameVersion = capture(webPage, "<h3>(Minecraft .+?) ?.*?</h3>", pos);
        GameVersion nextGameVersion;
        auto it = re.globalMatch(webPage);
        while (it.hasNext()) {
            auto match = it.next();
            if(auto gv = capture(webPage, "<h3>(Minecraft .+?) ?.*?</h3>", pos); gv != nextGameVersion){
                if(nextGameVersion != GameVersion::Any) gameVersion = nextGameVersion;
                nextGameVersion = gv;
            }
            list << ReplayModInfo::fromHtml(match.captured(1), gameVersion);
        }
        callback(list);
    });
}
