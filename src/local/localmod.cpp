#include "localmod.h"

#include "local/localmodpath.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthapi.h"
#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "config.h"

#include <QDir>
#include <iterator>
#include <algorithm>

LocalMod::LocalMod(QObject *parent, const LocalModInfo &info) :
    QObject(parent),
    curseforgeAPI_(CurseforgeAPI::api()),
    modrinthAPI_(ModrinthAPI::api()),
    modInfo_(info)
{}

LocalMod::LocalMod(LocalModPath *parent, const LocalModInfo &info) :
    QObject(parent),
    curseforgeAPI_(parent->curseforgeAPI()),
    modrinthAPI_(parent->modrinthAPI()),
    modInfo_(info)
{}

const LocalModInfo &LocalMod::modInfo() const
{
    return modInfo_;
}

void LocalMod::setModInfo(const LocalModInfo &newModInfo)
{
    modInfo_ = newModInfo;
    emit modInfoUpdated();
}

CurseforgeMod *LocalMod::curseforgeMod() const
{
    return curseforgeMod_;
}

void LocalMod::setCurseforgeMod(CurseforgeMod *newCurseforgeMod)
{
    curseforgeMod_ = newCurseforgeMod;
}

void LocalMod::searchOnWebsite()
{
    auto count = std::make_shared<int>(2);
    auto hasWeb = std::make_shared<bool>(false);
    auto foo = [=](bool bl){
        if(bl) *hasWeb = true;
        if(--(*count) == 0) emit websiteReady(*hasWeb);
    };

    emit checkWebsiteStarted();
    connect(this, &LocalMod::curseforgeReady, foo);
    searchOnCurseforge();
    connect(this, &LocalMod::modrinthReady, foo);
    searchOnModrinth();
}

void LocalMod::searchOnCurseforge()
{
    emit checkCurseforgeStarted();
    curseforgeAPI_->getIdByFingerprint(modInfo_.murmurhash(), [=](int id, auto fileInfo, const auto &fileList){
        CurseforgeModInfo modInfo(id);
        modInfo.setLatestFiles(fileList);
        curseforgeMod_ = new CurseforgeMod(this, modInfo);
        curseforgeUpdate_.setCurrentFileInfo(fileInfo);
        emit curseforgeReady(true);
    }, [=]{
        emit curseforgeReady(false);
    });
}

void LocalMod::searchOnModrinth()
{
    emit checkModrinthStarted();
    modrinthAPI_->getVersionFileBySha1(modInfo_.sha1(), [=](const auto &fileInfo){
        ModrinthModInfo modInfo(fileInfo.modId());
        modrinthMod_ = new ModrinthMod(this, modInfo);
        modrinthUpdate_.setCurrentFileInfo(fileInfo);
        emit modrinthReady(true);
    }, [=]{
        emit modrinthReady(false);
    });
}

void LocalMod::checkUpdates(const GameVersion &targetVersion, ModLoaderType::Type targetType)
{
    emit checkUpdatesStarted();

    int sourceCount = 0;
    Config config;
    if(config.getUseCurseforgeUpdate()) sourceCount++;
    if(config.getUseModrinthUpdate()) sourceCount++;
    if(sourceCount == 0){
        emit updateReady(false);
        return;
    }
    auto count = std::make_shared<int>(sourceCount);
    auto needUpdate = std::make_shared<bool>(false);
    auto foo = [=](bool bl){
        if(bl) *needUpdate = true;
        if(--(*count) == 0) emit updateReady(*needUpdate);
    };

    if(config.getUseCurseforgeUpdate()){
        connect(this, &LocalMod::curseforgeUpdateReady, foo);
        checkCurseforgeUpdate(targetVersion, targetType);
    }
    if(config.getUseModrinthUpdate()){
        connect(this, &LocalMod::modrinthUpdateReady, foo);
        checkModrinthUpdate(targetVersion, targetType);
    }
}

void LocalMod::checkCurseforgeUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType)
{
    if(!curseforgeUpdate_.isCurrentAvailable()){
        emit curseforgeUpdateReady(false);
        return;
    }

    emit checkCurseforgeUpdateStarted();

    //update file list
    auto updateFileList = [=]{
         bool bl = curseforgeUpdate_.findUpdate(curseforgeMod_->modInfo().allFileList(), targetVersion, targetType);
         emit curseforgeUpdateReady(bl);
    };

    if(!curseforgeMod_->modInfo().allFileList().isEmpty())
        updateFileList();
    else {
        curseforgeMod_->acquireAllFileList();
        connect(curseforgeMod_, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}

void LocalMod::checkModrinthUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType)
{
    if(!modrinthUpdate_.isCurrentAvailable()){
        emit modrinthUpdateReady(false);
        return;
    }

    emit checkModrinthUpdateStarted();

    //update file list
    auto updateFileList = [=]{
         bool bl = modrinthUpdate_.findUpdate(modrinthMod_->modInfo().fileList(), targetVersion, targetType);
         emit modrinthUpdateReady(bl);
    };

    auto updateFullInfo = [=]{
        if(!modrinthMod_->modInfo().fileList().isEmpty())
            updateFileList();
        else {
            modrinthMod_->acquireFileList();
            connect(modrinthMod_, &ModrinthMod::fileListReady, this, updateFileList);
        }
    };

    if(modrinthMod_->modInfo().hasFullInfo())
        updateFullInfo();
    else {
        modrinthMod_->acquireFullInfo();
        connect(modrinthMod_, &ModrinthMod::fullInfoReady, this, updateFullInfo);
    }
}

LocalMod::ModWebsiteType LocalMod::defaultUpdateType() const
{
    auto list = updateTypes();
    return list.isEmpty()? ModWebsiteType::None : list.first();
}

QList<LocalMod::ModWebsiteType> LocalMod::updateTypes() const
{
    auto bl1 = curseforgeUpdate_.isUpdateAvailable();
    auto bl2 = modrinthUpdate_.isUpdateAvailable();

    if(bl1 && !bl2)
        return { ModWebsiteType::Curseforge };
    else if(!bl1 && bl2)
        return { ModWebsiteType::Modrinth };
    else if(bl1 && bl2){
        if(curseforgeUpdate_.fileDate() > modrinthUpdate_.fileDate())
            return { ModWebsiteType::Curseforge, ModWebsiteType::Modrinth };
        else
            return { ModWebsiteType::Modrinth, ModWebsiteType::Curseforge};
    } else
        return {};
}

QPair<QString, QString> LocalMod::updateNames(ModWebsiteType type) const
{
    if(type == Curseforge)
        return curseforgeUpdate_.updateNames();
    else if(type == Modrinth)
        return modrinthUpdate_.updateNames();
    else
        return QPair("", "");
}

void LocalMod::update(ModWebsiteType type)
{
    if(type == None) return;
    emit updateStarted();

    auto path = QFileInfo(modInfo_.path()).absolutePath();
    auto callback = [=](const auto &newInfo){
        auto oldPath = modInfo_.path();
        auto newPath = QDir(path).absoluteFilePath(newInfo.fileName());

        auto newModInfo = LocalModInfo(newPath);
        //TODO other mod loader
        if(!newModInfo.isFabricMod()){
            emit updateFinished(false);
            return false;
        }

        //deal with old mod file
        auto postUpdate = Config().getPostUpdate();
        QFile file(oldPath);
        if(postUpdate == Config::Delete){
            //remove old file
            file.remove();

            //update info
            setModInfo(LocalModInfo(newPath));
        } else if(postUpdate == Config::Keep){
            //rename old file
            file.rename(file.fileName() + ".old");

            //update info
            setModInfo(newModInfo);
        }
        emit updateFinished(true);
        return true;
    };

    ModDownloader *downloader;

    if(type == ModWebsiteType::Curseforge)
        downloader = curseforgeUpdate_.update(path, callback);
    else if(type == ModWebsiteType::Modrinth)
        downloader = modrinthUpdate_.update(path, callback);

    connect(downloader, &ModDownloader::downloadProgress, this, &LocalMod::updateProgress);
}

qint64 LocalMod::updateSize(ModWebsiteType type) const
{
    switch (type) {
    case Curseforge:
        return curseforgeUpdate_.size();
    case Modrinth:
        return modrinthUpdate_.size();
    case None:
        return 0;
    }
}

CurseforgeAPI *LocalMod::curseforgeAPI() const
{
    return curseforgeAPI_;
}

ModrinthAPI *LocalMod::modrinthAPI() const
{
    return modrinthAPI_;
}

ModrinthMod *LocalMod::modrinthMod() const
{
    return modrinthMod_;
}

void LocalMod::setModrinthMod(ModrinthMod *newModrinthMod)
{
    modrinthMod_ = newModrinthMod;
}
