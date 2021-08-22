#include "localmod.h"

#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"

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
    CurseforgeAPI::getIdByFingerprint(localModInfo.getMurmurhash(), [=](int id, auto file, const auto &fileList){
        CurseforgeModInfo modInfo(id);
        modInfo.setLatestFiles(fileList);
        curseforgeMod = new CurseforgeMod(this, modInfo);
        currentCurseforgeFileInfo = file;
        emit curseforgeReady();
    });
}

void LocalMod::findUpdate(const GameVersion &version, ModLoaderType::Type loaderType)
{
    //update file list
    auto updateFileList = [=]{
         auto& list = curseforgeMod->getModInfo().getAllFiles();
         //select mod file for same game versions and mod loader type
         //TODO: ignore sub version
         QList<CurseforgeFileInfo> list2;
         std::insert_iterator<QList<CurseforgeFileInfo>> iter(list2, list2.begin());
         std::copy_if(list.cbegin(), list.cend(), iter, [=](const auto &file){
             return file.getGameVersions().contains(version) &&
                     (file.getModLoaders().isEmpty() || file.getModLoaders().contains(loaderType));
         });
         if(list2.isEmpty()) return ;
         auto resultIter = std::max_element(list2.cbegin(), list2.cend(), [=](const auto &file1, const auto &file2){
             return file1.getFileDate() < file2.getFileDate();
         });
         if(currentCurseforgeFileInfo.getDisplayName() != resultIter->getDisplayName()){
             qDebug() << localModInfo.getName() << ":" << currentCurseforgeFileInfo.getDisplayName() << "->" << resultIter->getDisplayName();
             qDebug() << resultIter->getGameVersions();
             emit needUpdate(*resultIter);
         }
    };

    if(!curseforgeMod->getModInfo().getAllFiles().isEmpty())
        updateFileList();
    else {
        curseforgeMod->acquireAllFileList();
        connect(curseforgeMod, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}
