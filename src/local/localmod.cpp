#include "localmod.h"

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
    modInfo_(info)
{

}

const LocalModInfo &LocalMod::modInfo() const
{
    return modInfo_;
}

CurseforgeMod *LocalMod::curseforgeMod() const
{
    return curseforgeMod_;
}

void LocalMod::setCurseforgeMod(CurseforgeMod *newCurseforgeMod)
{
    curseforgeMod_ = newCurseforgeMod;
}

void LocalMod::searchOnCurseforge()
{
    emit checkCurseforgeStarted();
    CurseforgeAPI::getIdByFingerprint(modInfo_.murmurhash(), [=](int id, auto file, const auto &fileList){
        CurseforgeModInfo modInfo(id);
        modInfo.setLatestFiles(fileList);
        curseforgeMod_ = new CurseforgeMod(this, modInfo);
        currentCurseforgeFileInfo_.emplace(file);
        emit curseforgeReady(true);
    }, [=]{
        emit curseforgeReady(false);
    });
}

void LocalMod::searchOnModrinth()
{
    emit checkModrinthStarted();
    ModrinthAPI::getVersionFileBySha1(modInfo_.sha1(), [=](const auto &fileInfo){
        ModrinthModInfo modInfo(fileInfo.modId());
        modrinthMod_ = new ModrinthMod(this, modInfo);
        currentModrinthFileInfo_.emplace(fileInfo);
        emit modrinthReady(true);
    }, [=]{
        emit modrinthReady(false);
    });
}

void LocalMod::checkCurseforgeUpdate(const GameVersion &mainVersion, ModLoaderType::Type loaderType)
{
    emit checkCurseforgeUpdateStarted();

    //update file list
    auto updateFileList = [=]{
         auto& list = curseforgeMod_->modInfo().allFileList();
         //select mod file for same game versions and mod loader type
         QList<CurseforgeFileInfo> list2;
         std::insert_iterator<QList<CurseforgeFileInfo>> iter(list2, list2.begin());
         std::copy_if(list.cbegin(), list.cend(), iter, [=](const auto &file){
             auto versionCheck = false;
             for(const auto & version : file.gameVersions())
                 if(version.mainVersion() == mainVersion)
                     versionCheck = true;
             return versionCheck && (file.loaderTypes().isEmpty() || file.loaderTypes().contains(loaderType));
         });
         if(list2.isEmpty()) return ;
         auto resultIter = std::max_element(list2.cbegin(), list2.cend(), [=](const auto &file1, const auto &file2){
             return file1.fileDate() < file2.fileDate();
         });
         //currentCurseforgeFileInfo should already have value before this function called
         if(currentCurseforgeFileInfo_.value().displayName() != resultIter->displayName()){
//             qDebug() << localModInfo.getName() << ":" << currentCurseforgeFileInfo.value().getDisplayName() << "->" << resultIter->getDisplayName();
             updateCurseforgeFileInfo_.emplace(*resultIter);
             emit curseforgeUpdateReady(true);
         } else
             emit curseforgeUpdateReady(false);
    };

    if(!curseforgeMod_->modInfo().allFileList().isEmpty())
        updateFileList();
    else {
        curseforgeMod_->acquireAllFileList();
        connect(curseforgeMod_, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}

void LocalMod::checkModrinthUpdate(const GameVersion &mainVersion, ModLoaderType::Type loaderType)
{
    emit checkModrinthUpdateStarted();

    //update file list
    auto updateFileList = [=]{
         auto& list = modrinthMod_->modInfo().fileList();
         //select mod file for same game versions and mod loader type
         QList<ModrinthFileInfo> list2;
         std::insert_iterator<QList<ModrinthFileInfo>> iter(list2, list2.begin());
         std::copy_if(list.cbegin(), list.cend(), iter, [=](const auto &file){
             auto versionCheck = false;
             for(const auto & version : file.gameVersions())
                 if(version.mainVersion() == mainVersion)
                     versionCheck = true;
             return versionCheck && (file.loaderTypes().isEmpty() || file.loaderTypes().contains(loaderType));
         });
         if(list2.isEmpty()) return ;
         auto resultIter = std::max_element(list2.cbegin(), list2.cend(), [=](const auto &file1, const auto &file2){
             return file1.fileDate() < file2.fileDate();
         });
         //currentModrinthFileInfo should already have value before this function called
         if(currentModrinthFileInfo_.value().displayName() != resultIter->displayName()){
             qDebug() << modInfo_.name() << ":" << currentModrinthFileInfo_.value().displayName() << "->" << resultIter->displayName();
             updateModrinthFileInfo_.emplace(*resultIter);
             emit modrinthUpdateReady(true);
         } else
             emit modrinthUpdateReady(false);
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

LocalMod::ModWebsiteType LocalMod::updateType() const
{
    auto bl1 = updateCurseforgeFileInfo_.has_value();
    auto bl2 = updateModrinthFileInfo_.has_value();

    if(bl1 && !bl2)
        return ModWebsiteType::Curseforge;
    else if(!bl1 && bl2)
        return ModWebsiteType::Modrinth;
    else if(bl1 && bl2){
        if(updateCurseforgeFileInfo_.value().fileDate() > updateModrinthFileInfo_.value().fileDate())
            return ModWebsiteType::Curseforge;
        else
            return ModWebsiteType::Modrinth;
    } else
        return ModWebsiteType::None;
}

void LocalMod::update(ModWebsiteType type)
{
    emit updateStarted();
    auto updateFunc = [=](auto mod, auto &oldFileInfo, auto &newFileInfo){
        QDir dir = modInfo_.path();
        //to dir
        dir.cdUp();
        DownloadManager::addModupdate(std::make_shared<std::remove_reference_t<decltype(newFileInfo.value())>>(newFileInfo.value()), dir.absolutePath(), [=]{
            //check download
            //...

            auto oldPath = modInfo_.path();
            QDir dir(oldPath);
            dir.cdUp();
            auto newPath = dir.absoluteFilePath(newFileInfo->fileName());

            //delete old mod file
            if(Config().getDeleteOld()){
                QFile file(oldPath);
                file.remove();

                //update info
                modInfo_.acquireInfo(newPath);

                //TODO
            }
            emit updateFinished();
        });
    };

    if(type == ModWebsiteType::Curseforge){
        updateFunc(curseforgeMod_, currentCurseforgeFileInfo_, updateCurseforgeFileInfo_);
        if(Config().getDeleteOld()){
        //update file info
            currentCurseforgeFileInfo_.emplace(updateCurseforgeFileInfo_.value());
            updateCurseforgeFileInfo_.reset();
        }
    }
    else if(type == ModWebsiteType::Modrinth){
        updateFunc(modrinthMod_, currentModrinthFileInfo_, updateModrinthFileInfo_);
        if(Config().getDeleteOld()){
            //update file info
            currentModrinthFileInfo_.emplace(updateModrinthFileInfo_.value());
            updateModrinthFileInfo_.reset();
        }
    }
    else
        qDebug() << "Nothing to update";

}

std::optional<CurseforgeFileInfo> LocalMod::currentCurseforgeFileInfo() const
{
    return currentCurseforgeFileInfo_;
}

std::optional<CurseforgeFileInfo> LocalMod::updateCurseforgeFileInfo() const
{
    return updateCurseforgeFileInfo_;
}

std::optional<ModrinthFileInfo> LocalMod::currentModrinthFileInfo() const
{
    return currentModrinthFileInfo_;
}

std::optional<ModrinthFileInfo> LocalMod::updateModrinthFileInfo() const
{
    return updateModrinthFileInfo_;
}

ModrinthMod *LocalMod::modrinthMod() const
{
    return modrinthMod_;
}

void LocalMod::setModrinthMod(ModrinthMod *newModrinthMod)
{
    modrinthMod_ = newModrinthMod;
}
