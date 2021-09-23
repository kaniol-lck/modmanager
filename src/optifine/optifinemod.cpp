#include "optifinemod.h"

#include "optifineapi.h"

OptifineMod::OptifineMod(QObject *parent) :
    QObject(parent),
    api_(OptifineAPI::api())
{}

OptifineMod::OptifineMod(QObject *parent, const OptifineModInfo &info) :
    QObject(parent),
    api_(OptifineAPI::api()),
    modInfo_(info)
{}

void OptifineMod::acquireDownloadUrl()
{
    if(gettingDownloadUrl_) return;
    gettingDownloadUrl_ = true;
    api_->getDownloadUrl(modInfo_.fileName(), [=](const auto &downloadUrl){
        modInfo_.downloadUrl_ = downloadUrl;
        gettingDownloadUrl_ = false;
        emit downloadUrlReady();
    });
}

const OptifineModInfo &OptifineMod::modInfo() const
{
    return modInfo_;
}
