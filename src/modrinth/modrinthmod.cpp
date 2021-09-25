#include "modrinthmod.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>

#include "modrinthapi.h"
#include "local/localmod.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"

ModrinthMod::ModrinthMod(LocalMod *parent, const QString &id) :
    QObject(parent),
    api_(parent->modrinthAPI()),
    modInfo_(id)
{}

ModrinthMod::ModrinthMod(QObject *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(ModrinthAPI::api()),
    modInfo_(info)
{}

ModrinthMod::ModrinthMod(LocalMod *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(parent->modrinthAPI()),
    modInfo_(info)
{}

ModrinthModInfo ModrinthMod::modInfo() const
{
    return modInfo_;
}

void ModrinthMod::acquireAuthor()
{
    if(modInfo_.authorId_.isEmpty() || gettingAuthor_) return;
    gettingAuthor_ = true;
    api_->getAuthor(modInfo_.modId_, [=](const auto &author){
        gettingAuthor_ = false;
        modInfo_.author_ = author;
        emit authorReady();
    });
}

void ModrinthMod::acquireIcon()
{
    if(modInfo_.iconUrl_.isEmpty() || gettingIcon_) return;
    gettingIcon_ = true;
    QNetworkRequest request(modInfo_.iconUrl_);
    static QNetworkAccessManager accessManager;
    auto reply = accessManager.get(request);
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
    api_->getInfo(modInfo_.modId_, [=](const auto &newInfo){
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

    api_->getVersions(modInfo_.modId_, [=](const auto &files){
        gettingFileList_ = false;
        modInfo_.fileList_ = files;
        emit fileListReady();
    });
}
