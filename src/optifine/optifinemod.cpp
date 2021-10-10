#include "optifinemod.h"

#include "optifineapi.h"
#include "bmclapi.h"
#include "config.hpp"

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
    if(gettingDownloadUrl_) return;
    gettingDownloadUrl_ = true;
    auto callback = [=](const auto &downloadUrl){
        modInfo_.downloadUrl_ = downloadUrl;
        gettingDownloadUrl_ = false;
        emit downloadUrlReady();
    };
    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official)
        api_->getDownloadUrl(modInfo_.fileName(), callback);
    else if(source == Config::OptifineSourceType::BMCLAPI)
        bmclapi_->getOptifineDownloadUrl(modInfo_, callback);
}

const OptifineModInfo &OptifineMod::modInfo() const
{
    return modInfo_;
}
