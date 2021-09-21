#include "localmod.h"

#include "local/localmodpath.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthapi.h"
#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "util/tutil.hpp"
#include "config.h"

#include <QDir>
#include <QJsonObject>
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
        curseforgeUpdate_.setCurrentFileInfo(fileInfo);
        emit modCacheUpdated();
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
        modrinthUpdate_.setCurrentFileInfo(fileInfo);
        emit modrinthReady(true);
    }, [=]{
        emit modrinthReady(false);
    });
}

void LocalMod::checkUpdates(const GameVersion &targetVersion, ModLoaderType::Type targetType)
{
    //clear update cache
    modrinthUpdate_.reset();
    curseforgeUpdate_.reset();

    emit checkUpdatesStarted();

    Config config;
    auto count = std::make_shared<int>(0);
    auto foo = [=](bool){
        if(--(*count) == 0){
            emit updateReady(defaultUpdateType());
        }
    };
    bool noSource = true;
    if(config.getUseCurseforgeUpdate() && curseforgeUpdate_.currentFileInfo()){
        (*count)++;
        connect(this, &LocalMod::curseforgeUpdateReady, foo);
        checkCurseforgeUpdate(targetVersion, targetType);
        noSource = false;
    }
    if(config.getUseModrinthUpdate() && modrinthUpdate_.currentFileInfo()){
        (*count)++;
        connect(this, &LocalMod::modrinthUpdateReady, foo);
        checkModrinthUpdate(targetVersion, targetType);
        noSource = false;
    }
    if(noSource) emit updateReady(ModWebsiteType::None);
}

void LocalMod::checkCurseforgeUpdate(const GameVersion &targetVersion, ModLoaderType::Type targetType)
{
    if(!curseforgeMod_){
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
    if(!modrinthMod_){
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
    bool bl1(curseforgeUpdate_.updateFileInfo());
    bool bl2(modrinthUpdate_.updateFileInfo());

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
    auto callback1 = [=](const auto &newInfo){
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

        if(type == Curseforge)
            modrinthUpdate_.reset(true);
        if(type == Modrinth)
            curseforgeUpdate_.reset(true);

        modFile_ = file;
        emit modFileUpdated();

        return true;
    };
    auto callback2 = [=]{
        emit modCacheUpdated();
        emit updateFinished(true);
    };

    ModDownloader *downloader;

    if(type == ModWebsiteType::Curseforge){
        if(curseforgeUpdate_.updateFileInfo())
            downloader = curseforgeUpdate_.update(path, modFile_->commonInfo()->iconBytes(), callback1, callback2);
        else
            downloader = curseforgeUpdate_.update(path, modFile_->commonInfo()->iconBytes(), callback1, callback2);
    }
    else if(type == ModWebsiteType::Modrinth){
        if(modrinthUpdate_.updateFileInfo())
            downloader = modrinthUpdate_.update(path, modFile_->commonInfo()->iconBytes(), callback1, callback2);
        else
            downloader = modrinthUpdate_.update(path, modFile_->commonInfo()->iconBytes(), callback1, callback2);
    }

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
    //TODO: refresh update info;
    auto disabled = isDisabled();
    if(disabled) setEnabled(true);
    file->removeOld();
    oldFiles_.removeAll(file);
    modFile_->addOld();
    oldFiles_ << modFile_;
    modFile_ = file;
    if(disabled) setEnabled(false);
    emit modFileUpdated();
}

void LocalMod::deleteAllOld()
{
    for(auto &oldFile : qAsConst(oldFiles_))
        oldFile->remove();
    oldFiles_.clear();
}

bool LocalMod::isDisabled()
{
    return modFile_->type() == LocalModFile::Disabled;
}

bool LocalMod::setEnabled(bool enabled)
{
    auto bl = modFile_->setEnabled(enabled);
    if(bl)
        emit modFileUpdated();
    return bl;
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

void LocalMod::setCurseforgeId(int id)
{
    curseforgeMod_ = new CurseforgeMod(this, id);
    emit curseforgeReady(true);
}

void LocalMod::setModrinthId(const QString &id)
{
    modrinthMod_ = new ModrinthMod(this, id);
    emit modrinthReady(true);
}

const QString &LocalMod::alias() const
{
    return alias_;
}

void LocalMod::setAlias(const QString &newAlias)
{
    alias_ = newAlias;
}

QJsonObject LocalMod::toJsonObject() const
{
    QJsonObject object;

    if(!alias_.isEmpty())
        object.insert("alias", alias_);
    if(isFeatured_)
        object.insert("featured", isFeatured_);

    if(curseforgeMod_){
        QJsonObject curseforgeObject;
        curseforgeObject.insert("id", curseforgeMod_->modInfo().id());
        if(curseforgeUpdate_.currentFileInfo())
            curseforgeObject.insert("currentFileInfo", curseforgeUpdate_.currentFileInfo()->toJsonObject());
        if(curseforgeUpdate_.updateFileInfo())
            curseforgeObject.insert("updateFileInfo", curseforgeUpdate_.updateFileInfo()->toJsonObject());
        object.insert("curseforge", curseforgeObject);
    }

    if(modrinthMod_){
        QJsonObject modrinthObject;
        modrinthObject.insert("id", modrinthMod_->modInfo().id());
        if(modrinthUpdate_.currentFileInfo())
            modrinthObject.insert("currentFileInfo", modrinthUpdate_.currentFileInfo()->toJsonObject());
        if(modrinthUpdate_.updateFileInfo())
            modrinthObject.insert("updateFileInfo", modrinthUpdate_.updateFileInfo()->toJsonObject());
        object.insert("modrinth", modrinthObject);
    }

    return object;
}

void LocalMod::restore(const QVariant &variant)
{
    alias_ = value(variant, "alias").toString();
    isFeatured_ = value(variant, "featured").toBool();
    if(contains(variant, "curseforge")){
        setCurseforgeId(value(variant, "curseforge", "id").toInt());
        if(contains(value(variant, "curseforge"), "currentFileInfo"))
            curseforgeUpdate_.setCurrentFileInfo(CurseforgeFileInfo::fromVariant(value(variant, "curseforge", "currentFileInfo")));
        if(contains(value(variant, "curseforge"), "updateFileInfo"))
            curseforgeUpdate_.setUpdateFileInfo(CurseforgeFileInfo::fromVariant(value(variant, "curseforge", "updateFileInfo")));
    }
    if(contains(variant, "modrinth")){
        setModrinthId(value(variant, "modrinth", "id").toString());
        if(contains(value(variant, "modrinth"), "currentFileInfo"))
            modrinthUpdate_.setCurrentFileInfo(ModrinthFileInfo::fromVariant(value(variant, "modrinth", "currentFileInfo")));
        if(contains(value(variant, "modrinth"), "updateFileInfo"))
            modrinthUpdate_.setUpdateFileInfo(ModrinthFileInfo::fromVariant(value(variant, "modrinth", "updateFileInfo")));
    }
}

bool LocalMod::isFeatured() const
{
    return isFeatured_;
}

void LocalMod::setFeatured(bool featured)
{
    isFeatured_ = featured;
    emit modCacheUpdated();
    emit modFileUpdated();
}

ModrinthMod *LocalMod::modrinthMod() const
{
    return modrinthMod_;
}
