#include "optifinemod.h"

#include "optifineapi.h"
#include "bmclapi.h"
#include "config.hpp"
#include "util/funcutil.h"

OptifineMod::OptifineMod(QObject *parent) :
    QObject(parent),
    api_(OptifineAPI::api()),
    bmclapi_(BMCLAPI::api())
{}

OptifineMod::OptifineMod(QObject *parent, const OptifineModInfo &info) :
    QObject(parent),
    api_(OptifineAPI::api()),
    bmclapi_(BMCLAPI::api()),
    modInfo_(info)
{}

void OptifineMod::acquireDownloadUrl()
{
    if(downloadUrlGetter_) return;
    auto callback = [=](const auto &downloadUrl){
        modInfo_.downloadUrl_ = downloadUrl;
        emit downloadUrlReady();
    };
    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official){
        downloadUrlGetter_ = api_->getDownloadUrl(modInfo_.fileName()).asUnique();
        downloadUrlGetter_->setOnFinished(this, callback);
    }else if(source == Config::OptifineSourceType::BMCLAPI){
        downloadUrlGetter_ = bmclapi_->getOptifineDownloadUrl(modInfo_).asUnique();
        downloadUrlGetter_->setOnFinished(this, callback);
    }
}

const OptifineModInfo &OptifineMod::modInfo() const
{
    return modInfo_;
}
