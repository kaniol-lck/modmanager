#include "curseforgemod.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "curseforge/curseforgeapi.h"
#include "util/tutil.hpp"
#include "util/funcutil.h"

CurseforgeMod::CurseforgeMod(QObject *parent, const CurseforgeModInfo &modInfo) :
    QObject(parent),
    modInfo_(modInfo)
{
}

void CurseforgeMod::acquireBasicInfo()
{
    if(gettingBasicInfo_) return;
    gettingBasicInfo_ = true;
    CurseforgeAPI::getInfo(modInfo_.id_, [=](const auto &info){
        gettingBasicInfo_ = false;
        modInfo_ = info;
        emit basicInfoReady();
        modInfo_.basicInfo_ = true;
    });
}

void CurseforgeMod::acquireIcon()
{
    if(modInfo_.iconUrl_.isEmpty() || gettingIcon_) return;
    gettingIcon_ = true;
    QNetworkRequest request(modInfo_.iconUrl_);
    auto reply = accessManager()->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        gettingIcon_ = false;
        if(reply->error() != QNetworkReply::NoError) return;
        modInfo_.iconBytes_ = reply->readAll();
        if(!modInfo_.iconBytes_.isEmpty())
            emit iconReady();
        reply->deleteLater();
    });
}

void CurseforgeMod::acquireDescription()
{
    if(gettingDescription_) return;
    gettingDescription_ = true;
    CurseforgeAPI::getDescription(modInfo_.id(), [=](const QString &description){
        gettingDescription_ = false;
        modInfo_.description_ = description;
        emit descriptionReady();
    });
}

void CurseforgeMod::acquireAllFileList()
{
    if(gettingAllFileList_) return;
    gettingAllFileList_ = true;
    CurseforgeAPI::getFiles(modInfo_.id(), [=](const QList<CurseforgeFileInfo> &fileList){
        gettingAllFileList_ = false;
        modInfo_.allFileList_ = fileList;
        emit allFileListReady();
    });
}

const CurseforgeModInfo &CurseforgeMod::modInfo() const
{
    return modInfo_;
}
