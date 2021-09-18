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
#include <tuple>

LocalMod::LocalMod(QObject *parent, LocalModFile *file) :
    QObject(parent),
    curseforgeAPI_(CurseforgeAPI::api()),
    modrinthAPI_(ModrinthAPI::api()),
    modFile_(file)
{}

LocalMod::LocalMod(LocalModPath *parent, LocalModFile *file) :
    QObject(parent),
    curseforgeAPI_(parent->curseforgeAPI()),
    modrinthAPI_(parent->modrinthAPI()),
    modFile_(file)
{}

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
    curseforgeAPI_->getIdByFingerprint(modFile_->murmurhash(), [=](int id, auto fileInfo, const auto &fileList){
        CurseforgeModInfo modInfo(id);
        modInfo.setLatestFiles(fileList);
        curseforgeMod_ = new CurseforgeMod(this, modInfo);
        curseforgeFileId_ = fileInfo.id();
        curseforgeUpdate_.setCurrentFileInfo(fileInfo);
        emit curseforgeReady(true);
    }, [=]{
        emit curseforgeReady(false);
    });
}

void LocalMod::searchOnModrinth()
{
    emit checkModrinthStarted();
    modrinthAPI_->getVersionFileBySha1(modFile_->sha1(), [=](const auto &fileInfo){
        ModrinthModInfo modInfo(fileInfo.modId());
        modrinthMod_ = new ModrinthMod(this, modInfo);
        modrinthFileId_ = fileInfo.id();
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
        emit updateReady(ModWebsiteType::None);
        return;
    }
    auto count = std::make_shared<int>(sourceCount);
    auto foo = [=](bool){
        if(--(*count) == 0) emit updateReady(defaultUpdateType());
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

    //always acquire
//    if(!curseforgeMod_->modInfo().allFileList().isEmpty())
//        updateFileList();
//    else {
        curseforgeMod_->acquireAllFileList();
        connect(curseforgeMod_, &CurseforgeMod::allFileListReady, this, updateFileList);
//    }
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

    //always acquire
//    if(modrinthMod_->modInfo().hasFullInfo())
//        updateFullInfo();
//    else {
        modrinthMod_->acquireFullInfo();
        connect(modrinthMod_, &ModrinthMod::fullInfoReady, this, updateFullInfo);
//    }
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

    auto path = QFileInfo(modFile_->path()).absolutePath();
    auto callback = [=](const auto &newInfo){
        auto newPath = QDir(path).absoluteFilePath(newInfo.fileName());

        auto file = new LocalModFile(this, newPath);
        file->loadInfo();
        //loader type mismatch
        if(file->loaderType() != modFile_->loaderType()){
            emit updateFinished(false);
            return false;
        }

        //deal with old mod file
        auto postUpdate = Config().getPostUpdate();
        if(postUpdate == Config::Delete){
            //remove old file
            modFile_->remove();
            modFile_->deleteLater();
        } else if(postUpdate == Config::Keep){
            modFile_->addOld();
            oldFiles_ << modFile_;
        }

        modFile_ = file;
        emit modFileUpdated();
        emit updateFinished(true);

        return true;
    };

    ModDownloader *downloader;

    if(type == ModWebsiteType::Curseforge)
        downloader = curseforgeUpdate_.update(path, modFile_->commonInfo()->iconBytes(), callback);
    else if(type == ModWebsiteType::Modrinth)
        downloader = modrinthUpdate_.update(path, modFile_->commonInfo()->iconBytes(), callback);

    connect(downloader, &ModDownloader::downloadProgress, this, &LocalMod::updateProgress);
}

qint64 LocalMod::updateSize(ModWebsiteType type) const
{
    if(type == Curseforge)
        return curseforgeUpdate_.size();
    if(type == Modrinth)
        return modrinthUpdate_.size();

    return 0;
}

CurseforgeAPI *LocalMod::curseforgeAPI() const
{
    return curseforgeAPI_;
}

ModrinthAPI *LocalMod::modrinthAPI() const
{
    return modrinthAPI_;
}

void LocalMod::addOldFile(LocalModFile *oldFile)
{
    oldFiles_ << oldFile;
    emit modFileUpdated();
}

void LocalMod::addDuplicateFile(LocalModFile *duplicateFile)
{
    duplicateFiles_ << duplicateFile;
}

void LocalMod::duplicateToOld()
{
    for(auto &file : qAsConst(duplicateFiles_)){
        file->addOld();
        oldFiles_ << file;
    }

    duplicateFiles_.clear();
    emit modFileUpdated();
}

void LocalMod::rollback(LocalModFile *file)
{
    file->removeOld();
    modFile_->addOld();
    modFile_ = file;
    emit modFileUpdated();
}

void LocalMod::deleteAllOld()
{
    for(auto &oldFile : qAsConst(oldFiles_))
        oldFile->remove();
    oldFiles_.clear();
}

void LocalMod::addDepend(std::tuple<QString, QString, std::optional<FabricModInfo> > modDepend)
{
    depends_ << modDepend;
}

void LocalMod::addConflict(std::tuple<QString, QString, FabricModInfo> modConflict)
{
    conflicts_ << modConflict;
}

void LocalMod::addBreak(std::tuple<QString, QString, FabricModInfo> modBreak)
{
    breaks_ << modBreak;
}

LocalModFile *LocalMod::modFile() const
{
    return modFile_;
}

const CommonModInfo *LocalMod::commonInfo() const
{
    return modFile_->commonInfo();
}

const QList<LocalModFile *> &LocalMod::oldFiles() const
{
    return oldFiles_;
}

const QList<LocalModFile *> &LocalMod::duplicateFiles() const
{
    return duplicateFiles_;
}

const Updatable<CurseforgeFileInfo> &LocalMod::curseforgeUpdate() const
{
    return curseforgeUpdate_;
}

const Updatable<ModrinthFileInfo> &LocalMod::modrinthUpdate() const
{
    return modrinthUpdate_;
}

const QList<std::tuple<QString, QString, std::optional<FabricModInfo> > > &LocalMod::depends() const
{
    return depends_;
}

const QList<std::tuple<QString, QString, FabricModInfo> > &LocalMod::conflicts() const
{
    return conflicts_;
}

const QList<std::tuple<QString, QString, FabricModInfo> > &LocalMod::breaks() const
{
    return breaks_;
}

int LocalMod::curseforgeFileId() const
{
    return curseforgeFileId_;
}

const QString &LocalMod::modrinthFileId() const
{
    return modrinthFileId_;
}

ModrinthMod *LocalMod::modrinthMod() const
{
    return modrinthMod_;
}
