#include "modrinthmod.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>

#include "modrinthapi.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"

ModrinthMod::ModrinthMod(QObject *parent, const ModrinthModInfo &info) :
    QObject(parent),
    modInfo_(info)
{

}

ModrinthModInfo ModrinthMod::modInfo() const
{
    return modInfo_;
}

void ModrinthMod::acquireIcon()
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

void ModrinthMod::acquireFullInfo()
{
    if(gettingFullInfo_) return;
    gettingFullInfo_ = true;
    ModrinthAPI::getInfo(modInfo_.modId_, [=](const auto &newInfo){
        gettingFullInfo_ = false;
        if(modInfo_.basicInfo_){
            modInfo_.description_ = newInfo.description_;
            modInfo_.versionList_ = newInfo.versionList_;
        } else
            modInfo_ = newInfo;
        emit fullInfoReady();
    });
}

void ModrinthMod::acquireFileList()
{
    if(gettingFileList_) return;
    gettingFileList_ = true;

    auto count = std::make_shared<int>(modInfo_.versionList_.size());

    for(const auto &version : qAsConst(modInfo_.versionList_)){
        ModrinthAPI::getVersion(version, [=](const auto &file){
            modInfo_.fileList_ << file;
            (*count)--;
            if(*count == 0){
                gettingFileList_ = false;
                emit fileListReady();
            }
        });
    }
}
