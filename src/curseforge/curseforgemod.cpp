#include "curseforgemod.h"

#include <QStandardPaths>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "download/assetcache.h"
#include "local/localmod.h"
#include "local/localmodpath.h"
#include "curseforge/curseforgeapi.h"
#include "util/tutil.hpp"
#include "util/funcutil.h"
#include "util/mmlogger.h"

CurseforgeMod::CurseforgeMod(QObject *parent, int id) :
    QObject(parent),
    api_(CurseforgeAPI::api()),
    modInfo_(id)
{
    addSubTagable(&modInfo_);
}

CurseforgeMod::CurseforgeMod(LocalMod *parent, int id) :
    QObject(parent),
    api_(parent->curseforgeAPI()),
    modInfo_(id)
{
    addSubTagable(&modInfo_);
}

CurseforgeMod::CurseforgeMod(QObject *parent, const CurseforgeModInfo &info) :
    QObject(parent),
    api_(CurseforgeAPI::api()),
    modInfo_(info)
{
    addSubTagable(&modInfo_);
}

CurseforgeMod::CurseforgeMod(LocalMod *parent, const CurseforgeModInfo &info) :
    QObject(parent),
    api_(parent->curseforgeAPI()),
    modInfo_(info)
{
    addSubTagable(&modInfo_);
}

CurseforgeMod::~CurseforgeMod()
{
    MMLogger::dtor(this) << modInfo_.id() << modInfo_.name();
}

void CurseforgeMod::acquireBasicInfo()
{
    if(basicInfoGetter_) return;
    basicInfoGetter_ = api_->getInfo(modInfo_.id_).asUnique();
    basicInfoGetter_->setOnFinished([=](const auto &info){
        modInfo_ = info;
        addSubTagable(&modInfo_);
        emit basicInfoReady();
        modInfo_.basicInfo_ = true;
    });
}

void CurseforgeMod::acquireIcon()
{
    if(!modInfo_.icon_.isNull() || modInfo_.iconUrl_.isEmpty() || gettingIcon_) return;
    gettingIcon_ = true;
    auto iconAsset = new AssetCache(this, modInfo_.iconUrl_, modInfo_.iconUrl_.fileName(), CurseforgeModInfo::cachePath());
    iconAsset->download();
    connect(iconAsset, &AssetCache::assetReady, this, [=]{
        modInfo_.icon_.load(iconAsset->destFilePath());
        gettingIcon_ = false;
        emit iconReady();
    });
}

void CurseforgeMod::acquireDescription()
{
    if(descriptionGetter_) return;
    descriptionGetter_ = api_->getDescription(modInfo_.id()).asUnique();
    descriptionGetter_->setOnFinished([=](const QString &description){
        modInfo_.description_ = description;
        emit descriptionReady();
    });
}

std::shared_ptr<Reply<QList<CurseforgeFileInfo>>> CurseforgeMod::acquireAllFileList()
{
    if(allFileListGetter_ && allFileListGetter_->isRunning()) return {};
    allFileListGetter_ = api_->getFiles(modInfo_.id()).asUnique();
    allFileListGetter_->setOnFinished([=](const QList<CurseforgeFileInfo> &fileList){
        modInfo_.allFileList_ = fileList;
        emit allFileListReady(fileList);
    }, [=]{
        emit allFileListReady({});
    });
    return allFileListGetter_;
}

const CurseforgeModInfo &CurseforgeMod::modInfo() const
{
    return modInfo_;
}

void CurseforgeMod::download(const CurseforgeFileInfo &fileInfo, LocalModPath *downloadPath)
{
    DownloadFileInfo info(fileInfo);
    info.setIcon(modInfo_.icon());
    info.setTitle(modInfo_.name());
    if(downloadPath)
        downloader_ = downloadPath->downloadNewMod(info);
    else{
        info.setPath(Config().getDownloadPath());
        downloader_ = DownloadManager::manager()->download(info);
    }
    emit downloadStarted();
}

QAria2Downloader *CurseforgeMod::downloader() const
{
    return downloader_;
}
