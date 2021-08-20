#include "curseforgemod.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "curseforge/curseforgeapi.h"
#include "util/tutil.hpp"
#include "util/downloader.h"

CurseforgeMod::CurseforgeMod(QObject *parent, QNetworkAccessManager *manager, const CurseforgeModInfo &modInfo) :
    QObject(parent),
    accessManager(manager),
    curseforgeModInfo(modInfo)
{
}

void CurseforgeMod::downloadThumbnail()
{
    if(curseforgeModInfo.thumbnailUrl.isEmpty()) return;

    QNetworkRequest request(curseforgeModInfo.thumbnailUrl);
    auto reply = accessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) return;
        curseforgeModInfo.thumbnailBytes = reply->readAll();
        if(!curseforgeModInfo.thumbnailBytes.isEmpty())
            emit thumbnailReady();
        reply->deleteLater();
    });
}

void CurseforgeMod::getDescription()
{
    CurseforgeAPI::getDescription(curseforgeModInfo.getId(), [=](const QString &description){
        curseforgeModInfo.description = description;
        emit descriptionReady();
    });
}

void CurseforgeMod::getAllFileList()
{
    CurseforgeAPI::getFiles(curseforgeModInfo.getId(), [=](const QList<CurseforgeFileInfo> &fileList){
        curseforgeModInfo.allFiles = fileList;
        emit allFileListReady();
    });
}

const CurseforgeModInfo &CurseforgeMod::getModInfo() const
{
    return curseforgeModInfo;
}
