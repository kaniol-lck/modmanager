#include "curseforgemod.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "curseforge/curseforgeapi.h"
#include "util/tutil.hpp"
#include "util/downloader.h"
#include "util/funcutil.h"

CurseforgeMod::CurseforgeMod(QObject *parent, const CurseforgeModInfo &modInfo) :
    QObject(parent),
    curseforgeModInfo(modInfo)
{
}

void CurseforgeMod::acquireBasicInfo()
{
    if(gettingBasicInfo) return;
    gettingBasicInfo = true;
    CurseforgeAPI::getInfo(curseforgeModInfo.id, [=](const auto &info){
        gettingBasicInfo = false;
        curseforgeModInfo = info;
        emit basicInfoReady();
        curseforgeModInfo.basicInfo = true;
    });
}

void CurseforgeMod::acquireIcon()
{
    if(curseforgeModInfo.iconUrl.isEmpty() || gettingIcon) return;
    gettingIcon = true;
    QNetworkRequest request(curseforgeModInfo.iconUrl);
    auto reply = accessManager()->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        gettingIcon = false;
        if(reply->error() != QNetworkReply::NoError) return;
        curseforgeModInfo.iconBytes = reply->readAll();
        if(!curseforgeModInfo.iconBytes.isEmpty())
            emit iconReady();
        reply->deleteLater();
    });
}

void CurseforgeMod::acquireDescription()
{
    if(gettingDescription) return;
    gettingDescription = true;
    CurseforgeAPI::getDescription(curseforgeModInfo.getId(), [=](const QString &description){
        gettingDescription = false;
        curseforgeModInfo.description = description;
        emit descriptionReady();
    });
}

void CurseforgeMod::acquireAllFileList()
{
    if(gettingAllFileList) return;
    gettingAllFileList = true;
    CurseforgeAPI::getFiles(curseforgeModInfo.getId(), [=](const QList<CurseforgeFileInfo> &fileList){
        gettingAllFileList = false;
        curseforgeModInfo.allFileList = fileList;
        emit allFileListReady();
    });
}

void CurseforgeMod::download(const CurseforgeFileInfo &fileInfo, const QDir &path)
{
    auto downloader = new Downloader(this);
    downloader->download(fileInfo.getDownloadUrl(), path, fileInfo.getFileName());
    connect(downloader, &Downloader::downloadProgress, this, &CurseforgeMod::downloadProgress);
    connect(downloader, &Downloader::finished, this, &CurseforgeMod::downloadFinished);
}

const CurseforgeModInfo &CurseforgeMod::getModInfo() const
{
    return curseforgeModInfo;
}
