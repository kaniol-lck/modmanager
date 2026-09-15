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
    // 列表里已经带出直链（BMCLAPI 的 versionList 有 filename，可在列表阶段就地拼出 URL），
    // 此时不必再为每个条目发一次请求，直接同步通知调用方。
    if(!modInfo_.downloadUrl().isEmpty()){
        emit downloadUrlReady();
        return;
    }
    if(downloadUrlGetter_) return;
    if(modInfo_.fileName().isEmpty()){
        // 连文件名都没有（网页/JSON 都没解析出来），发请求也拿不到有用的东西
        emit downloadUrlFailed();
        return;
    }
    auto callback = [=](const auto &downloadUrl){
        modInfo_.setDownloadUrl(downloadUrl);
        emit downloadUrlReady();
    };
    auto errorHandler = [=](auto){
        // 失败后必须允许重试：原实现把 downloadUrlGetter_ 一直留着，而本函数开头就是
        // if(downloadUrlGetter_) return; —— 一次失败就永久不再尝试，按钮一直是灰的。
        downloadUrlGetter_.reset();
        emit downloadUrlFailed();
    };
    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official){
        downloadUrlGetter_ = api_->getDownloadUrl(modInfo_.fileName()).asUnique();
        downloadUrlGetter_->setOnFinished(this, callback, errorHandler);
    }else if(source == Config::OptifineSourceType::BMCLAPI){
        downloadUrlGetter_ = bmclapi_->getOptifineDownloadUrl(modInfo_).asUnique();
        downloadUrlGetter_->setOnFinished(this, callback, errorHandler);
    }
}

const OptifineModInfo &OptifineMod::modInfo() const
{
    return modInfo_;
}
