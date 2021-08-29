#include "modrinthmod.h"

#include <QNetworkRequest>
#include <QNetworkReply>

#include "modrinthapi.h"
#include "util/funcutil.h"

ModrinthMod::ModrinthMod(QObject *parent, const ModrinthModInfo &info) :
    QObject(parent),
    modInfo(info)
{

}

ModrinthModInfo ModrinthMod::getModInfo() const
{
    return modInfo;
}

void ModrinthMod::acquireIcon()
{
    if(modInfo.iconUrl.isEmpty() || gettingIcon) return;
    gettingIcon = true;
    QNetworkRequest request(modInfo.iconUrl);
    auto reply = accessManager()->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        gettingIcon = false;
        if(reply->error() != QNetworkReply::NoError) return;
        modInfo.iconBytes = reply->readAll();
        if(!modInfo.iconBytes.isEmpty())
            emit iconReady();
        reply->deleteLater();
    });
}

void ModrinthMod::acquireFullInfo()
{
    ModrinthAPI::getInfo(modInfo.id, [=](auto newInfo){
        modInfo = newInfo;
        emit fullInfoReady();
    });
}
