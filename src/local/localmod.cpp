#include "localmod.h"

#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthapi.h"
#include "modrinth/modrinthmod.h"
#include "util/downloader.h"
#include "config.h"

#include <iterator>
#include <algorithm>

LocalMod::LocalMod(QObject *parent, const LocalModInfo &info) :
    QObject(parent),
    localModInfo(info)
{

}

const LocalModInfo &LocalMod::getModInfo() const
{
    return localModInfo;
}

CurseforgeMod *LocalMod::getCurseforgeMod() const
{
    return curseforgeMod;
}

void LocalMod::setCurseforgeMod(CurseforgeMod *newCurseforgeMod)
{
    curseforgeMod = newCurseforgeMod;
}

void LocalMod::searchOnCurseforge()
{
    emit checkCurseforgeStarted();
    CurseforgeAPI::getIdByFingerprint(localModInfo.getMurmurhash(), [=](int id, auto file, const auto &fileList){
        CurseforgeModInfo modInfo(id);
        modInfo.setLatestFiles(fileList);
        curseforgeMod = new CurseforgeMod(this, modInfo);
        currentCurseforgeFileInfo.emplace(file);
        emit curseforgeReady(true);
    }, [=]{
        emit curseforgeReady(false);
    });
}

void LocalMod::searchOnModrinth()
{
    emit checkModrinthStarted();
    ModrinthAPI::getVersionFileBySha1(localModInfo.getSha1(), [=](const auto &fileInfo){
        ModrinthModInfo modInfo(fileInfo.getModId());
        modrinthMod = new ModrinthMod(this, modInfo);
        currentModrinthFileInfo.emplace(fileInfo);
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
         auto& list = curseforgeMod->getModInfo().getAllFileList();
         //select mod file for same game versions and mod loader type
         QList<CurseforgeFileInfo> list2;
         std::insert_iterator<QList<CurseforgeFileInfo>> iter(list2, list2.begin());
         std::copy_if(list.cbegin(), list.cend(), iter, [=](const auto &file){
             auto versionCheck = false;
             for(const auto & version : file.getGameVersions())
                 if(version.mainVersion() == mainVersion)
                     versionCheck = true;
             return versionCheck && (file.getModLoaders().isEmpty() || file.getModLoaders().contains(loaderType));
         });
         if(list2.isEmpty()) return ;
         auto resultIter = std::max_element(list2.cbegin(), list2.cend(), [=](const auto &file1, const auto &file2){
             return file1.getFileDate() < file2.getFileDate();
         });
         //currentCurseforgeFileInfo should already have value before this function called
         if(currentCurseforgeFileInfo.value().getDisplayName() != resultIter->getDisplayName()){
//             qDebug() << localModInfo.getName() << ":" << currentCurseforgeFileInfo.value().getDisplayName() << "->" << resultIter->getDisplayName();
             updateCurseforgeFileInfo.emplace(*resultIter);
             emit curseforgeUpdateReady(true);
         } else
             emit curseforgeUpdateReady(false);
    };

    if(!curseforgeMod->getModInfo().getAllFileList().isEmpty())
        updateFileList();
    else {
        curseforgeMod->acquireAllFileList();
        connect(curseforgeMod, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}

void LocalMod::update()
{
    if(!updateCurseforgeFileInfo.has_value()){
        qDebug() << localModInfo.getName() << "no update file.";
        return;
    }
    emit updateStarted();

    auto path = localModInfo.getModPath();
    //to dir
    path.cdUp();
    curseforgeMod->download(updateCurseforgeFileInfo.value(), path);

    connect(curseforgeMod, &CurseforgeMod::downloadProgress, this, &LocalMod::updateProgress);
    connect(curseforgeMod, &CurseforgeMod::downloadFinished, this, [=]{
        //check download
        //...

        auto oldPath = localModInfo.getModPath();
        QDir dir(oldPath);
        dir.cdUp();
        auto newPath = dir.absoluteFilePath(updateCurseforgeFileInfo->getFileName());

        //delete old mod file
        if(Config().getDeleteOld()){
            QFile file(oldPath.absolutePath());
            file.remove();

            //update info
            localModInfo.acquireInfo(newPath);

            //update file info
            currentCurseforgeFileInfo.emplace(updateCurseforgeFileInfo.value());
            updateCurseforgeFileInfo.reset();
        }
        emit updateFinished();
    });

}

std::optional<CurseforgeFileInfo> LocalMod::getCurrentCurseforgeFileInfo() const
{
    return currentCurseforgeFileInfo;
}

std::optional<CurseforgeFileInfo> LocalMod::getUpdateCurseforgeFileInfo() const
{
    return updateCurseforgeFileInfo;
}

std::optional<ModrinthFileInfo> LocalMod::getCurrentModrinthFileInfo() const
{
    return currentModrinthFileInfo;
}

std::optional<ModrinthFileInfo> LocalMod::getUpdateModrinthFileInfo() const
{
    return updateModrinthFileInfo;
}

ModrinthMod *LocalMod::getModrinthMod() const
{
    return modrinthMod;
}

void LocalMod::setModrinthMod(ModrinthMod *newModrinthMod)
{
    modrinthMod = newModrinthMod;
}
