#include "modrinthmod.h"

#include <QStandardPaths>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>

#include "modrinthapi.h"
#include "local/localmod.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"
#include "util/mmlogger.h"
#include "local/localmodpath.h"

ModrinthMod::ModrinthMod(LocalMod *parent, const QString &id) :
    QObject(parent),
    api_(parent->modrinthAPI())
{
    setModInfo(ModrinthModInfo(id));
}

ModrinthMod::ModrinthMod(QObject *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(ModrinthAPI::api())
{
    setModInfo(info);
}

ModrinthMod::ModrinthMod(LocalMod *parent, const ModrinthModInfo &info) :
    QObject(parent),
    api_(parent->modrinthAPI())
{
    setModInfo(info);
}

ModrinthMod::~ModrinthMod()
{
    MMLogger::dtor(this) << modInfo_.id() << modInfo_.name();
}

ModrinthModInfo ModrinthMod::modInfo() const
{
    return modInfo_;
}

//void ModrinthMod::acquireAuthor()
//{
//    if(modInfo_.authorId_.isEmpty() || gettingAuthor_) return;
//    gettingAuthor_ = true;
//    auto conn = api_->getAuthor(modInfo_.modId_, [=](const auto &author){
//        gettingAuthor_ = false;
//        modInfo_.author_ = author;
//        emit authorReady();
//    });
//    connect(this, &QObject::destroyed, this, [=]{
//        disconnect(conn);
//    });
//}

void ModrinthMod::acquireIcon()
{
    if(modInfo_.iconUrl_.isEmpty() || gettingIcon_) return;
    gettingIcon_ = true;
    QNetworkRequest request(modInfo_.iconUrl_);
    static QNetworkAccessManager accessManager;
    static QNetworkDiskCache diskCache;
    diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    accessManager.setCache(&diskCache);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
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

std::shared_ptr<Reply<ModrinthModInfo>> ModrinthMod::acquireFullInfo()
{
    if(fullInfoGetter_ && fullInfoGetter_->isRunning()) return {};
    fullInfoGetter_ = api_->getInfo(modInfo_.modId_).asShared();
    fullInfoGetter_->setOnFinished([=](const auto &newInfo){
        if(modInfo_.basicInfo_){
            modInfo_.description_ = newInfo.description_;
            modInfo_.versionList_ = newInfo.versionList_;
        } else
            modInfo_ = newInfo;
        emit fullInfoReady();
    });
    return fullInfoGetter_;
}

std::shared_ptr<Reply<QList<ModrinthFileInfo> > > ModrinthMod::acquireFileList()
{
    if(fileListGetter_ && fileListGetter_->isRunning()) return {};
    fileListGetter_ = api_->getVersions(modInfo_.modId_).asShared();
    fileListGetter_->setOnFinished([=](const auto &files){
        modInfo_.fileList_ = files;
        emit fileListReady(files);
    }, [=](auto){
        emit fileListReady({});
    });
    return fileListGetter_;
}

void ModrinthMod::download(const ModrinthFileInfo &fileInfo, LocalModPath *downloadPath)
{
    DownloadFileInfo info(fileInfo);
    QPixmap pixelmap;
    pixelmap.loadFromData(modInfo_.iconBytes());
    info.setIcon(pixelmap);
    info.setTitle(modInfo_.name());

    if(downloadPath)
        downloader_ = downloadPath->downloadNewMod(info);
    else{
        info.setPath(Config().getDownloadPath());
        downloader_ = DownloadManager::manager()->download(info);
    }
    emit downloadStarted();
}

void ModrinthMod::setModInfo(ModrinthModInfo newModInfo)
{
    modInfo_ = newModInfo;
    addSubTagable(&modInfo_);
}

QAria2Downloader *ModrinthMod::downloader() const
{
    return downloader_;
}
