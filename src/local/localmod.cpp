#include "localmod.h"

#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <iterator>
#include <algorithm>
#include <tuple>
#include <functional>

#include "local/localmodpath.h"
#include "local/idmapper.h"
#include "local/knownfile.h"
#include "local/localfilelinker.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthapi.h"
#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "util/tutil.hpp"
#include "config.hpp"
#include "util/mmlogger.h"
#include "util/funcutil.h"

LocalMod::LocalMod(QObject *parent, LocalModFile *file) :
    QObject(parent),
    curseforgeAPI_(CurseforgeAPI::api()),
    modrinthAPI_(ModrinthAPI::api())
{
//    connect(this, &LocalMod::tagsEditted, this, &LocalMod::modFileUpdated);
    setModFile(file);
}

LocalMod::LocalMod(LocalModPath *parent, LocalModFile *file) :
    QObject(parent),
    curseforgeAPI_(parent->curseforgeAPI()),
    modrinthAPI_(parent->modrinthAPI()),
    path_(parent),
    targetVersion_(parent->info().gameVersion()),
    targetLoaderType_(parent->info().loaderType())
{
    connect(this, &LocalMod::tagsEditted, path_, &LocalModPath::writeToFile);
    connect(this, &LocalMod::modCacheUpdated, path_, &LocalModPath::writeToFile);
    addSubTagable(path_);
    setModFile(file);
    connect(parent, &LocalModPath::infoUpdated, this, [=]{
        targetVersion_ = parent->info().gameVersion();
        targetLoaderType_ = parent->info().loaderType();
    });
}

LocalMod::~LocalMod()
{
    MMLogger::dtor(this) << commonInfo()->id();
}

CurseforgeMod *LocalMod::curseforgeMod() const
{
    return curseforgeMod_;
}

void LocalMod::setCurseforgeMod(CurseforgeMod *newCurseforgeMod)
{
    curseforgeMod_ = newCurseforgeMod;
    if(curseforgeMod_){
        curseforgeMod_->setParent(this);
        emit curseforgeReady(true);
        emit modCacheUpdated();
        emit modInfoChanged();
    }
}

void LocalMod::checkUpdates(bool force)
{
    //clear update cache
    modrinthUpdater_.reset();
    curseforgeUpdater_.reset();

    emit checkUpdatesStarted();

    Config config;
    auto count = std::make_shared<int>(0);
    auto doneCount = std::make_shared<int>(0);
    auto success = std::make_shared<bool>(false);
    auto foo = [=](bool hasUpdate[[maybe_unused]], bool success2){
        qDebug() << displayName() << "done:" << ++(*doneCount) << "/" << *count;
        if(success2) *success = true;
        if(*doneCount == *count){
            emit modCacheUpdated();
            emit updateReady(updateTypes(), *success);
        }
    };
    if(config.getUseCurseforgeUpdate() && curseforgeMod_ && modFile_->linker()->curseforgeFileInfo()){
        (*count)++;
        checkCurseforgeUpdate(force);
//        connect(this, &LocalMod::checkCancelled, disconnecter(
//        connect(this, &LocalMod::curseforgeUpdateReady, foo);
        connect(this, &LocalMod::curseforgeUpdateReady, [=](bool hasUpdate, bool success2){
            qDebug() << "curseforge finish: " << displayName();
            foo(hasUpdate, success2);
        });
    }
    if(config.getUseModrinthUpdate() && modrinthMod_ && modFile_->linker()->modrinthFileInfo()){
        (*count)++;
        checkModrinthUpdate(force);
//        connect(this, &LocalMod::checkCancelled, disconnecter(
//                    connect(this, &LocalMod::modrinthUpdateReady, foo)));
        connect(this, &LocalMod::modrinthUpdateReady, [=](bool hasUpdate, bool success2){
            qDebug() << "modrinth finish: " << displayName();
            foo(hasUpdate, success2);
        });
    }
    if(!*count) emit updateReady({});
}

void LocalMod::cancelChecking()
{
    emit checkCancelled();
}

void LocalMod::checkCurseforgeUpdate(bool force)
{
    if(!curseforgeMod_){
        emit curseforgeUpdateReady(false, false);
        return;
    }

    emit checkCurseforgeUpdateStarted();

    //update file list
    if(force || curseforgeMod_->modInfo().allFileList().isEmpty()){
        connect(this, &LocalMod::checkCancelled, disconnecter(
                    curseforgeMod_->acquireAllFileList([=](const QList<CurseforgeFileInfo> &fileList){
            bool bl = curseforgeUpdater_.findUpdate(modFile_->linker()->curseforgeFileInfo(), fileList, targetVersion_, targetLoaderType_);
            emit curseforgeUpdateReady(bl);
        }, [=]{
            emit curseforgeUpdateReady(false, false);
        })));
    }else{
        bool bl = curseforgeUpdater_.findUpdate(modFile_->linker()->curseforgeFileInfo(), curseforgeMod_->modInfo().allFileList(), targetVersion_, targetLoaderType_);
        emit curseforgeUpdateReady(bl);
    }
}

void LocalMod::checkModrinthUpdate(bool force)
{
    if(!modrinthMod_){
        emit modrinthUpdateReady(false, false);
        return;
    }

    emit checkModrinthUpdateStarted();
    auto updateFullInfo = [=]{
        if(!modrinthMod_->modInfo().fileList().isEmpty()){
            bool bl = modrinthUpdater_.findUpdate(modFile_->linker()->modrinthFileInfo(), modrinthMod_->modInfo().fileList(), targetVersion_, targetLoaderType_);
            emit modrinthUpdateReady(bl);
        }else {
            connect(this, &LocalMod::checkCancelled, disconnecter(
                        modrinthMod_->acquireFileList([=](const QList<ModrinthFileInfo> &fileList){
                bool bl = modrinthUpdater_.findUpdate(modFile_->linker()->modrinthFileInfo(), fileList, targetVersion_, targetLoaderType_);
                emit modrinthUpdateReady(bl);
            }, [=]{
                emit modrinthUpdateReady(false, false);
            })));
        }
    };

    //always acquire
//    if(modrinthMod_->modInfo().hasFullInfo())
//        updateFullInfo();
//    else {
    if(force || modrinthMod_->modInfo().fileList().isEmpty()){
        if(modrinthMod_->modInfo().hasFullInfo())
            updateFullInfo();
        else{
            modrinthMod_->acquireFullInfo();
            connect(modrinthMod_, &ModrinthMod::fullInfoReady, this, updateFullInfo);
        }
    }
    //    }
}

QAria2Downloader *LocalMod::downloadOldMod(DownloadFileInfo &info)
{
    //TODO: something wrong
    info.setPath(path_->info().path());
    auto downloader = DownloadManager::manager()->download(info);
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        QFileInfo fileInfo(path_->info().path(), info.fileName());
        if(!LocalModFile::availableSuffix.contains(fileInfo.suffix())) return;
        auto file = new LocalModFile(path_, fileInfo.absoluteFilePath());
        file->loadInfo();
        file->linker()->linkCached();
        qDebug() << file->addOld();
        addOldFile(file);
        emit modInfoChanged();
    });
    return downloader;
}

ModWebsiteType LocalMod::defaultUpdateType() const
{
    auto list = updateTypes();
    return list.isEmpty()? ModWebsiteType::None : list.first();
}

QList<ModWebsiteType> LocalMod::updateTypes() const
{
    bool bl1(curseforgeUpdater_.hasUpdate());
    bool bl2(modrinthUpdater_.hasUpdate());

    if(bl1 && !bl2)
        return { ModWebsiteType::Curseforge };
    else if(!bl1 && bl2)
        return { ModWebsiteType::Modrinth };
    else if(bl1 && bl2){
        if(curseforgeUpdater_.updateFileInfos().first().fileDate() > modrinthUpdater_.updateFileInfos().first().fileDate())
            return { ModWebsiteType::Curseforge, ModWebsiteType::Modrinth };
        else
            return { ModWebsiteType::Modrinth, ModWebsiteType::Curseforge};
    } else
        return {};
}

QAria2Downloader *LocalMod::update()
{
    switch (defaultUpdateType()) {
    case ModWebsiteType::None:
        return nullptr;
    case ModWebsiteType::Curseforge:
        return update(curseforgeUpdater_.updateFileInfos().first());
    case ModWebsiteType::Modrinth:
        return update(modrinthUpdater_.updateFileInfos().first());
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

void LocalMod::addOldFile(LocalModFile *oldFile)
{
    oldFile->setParent(this);
    oldFile->setMod(this);
    oldFiles_ << oldFile;
    emit modInfoChanged();
}

void LocalMod::addDuplicateFile(LocalModFile *duplicateFile)
{
    duplicateFile->setParent(this);
    duplicateFile->setMod(this);
    duplicateFiles_ << duplicateFile;
}

void LocalMod::duplicateToOld()
{
    for(auto &file : qAsConst(duplicateFiles_)){
        file->addOld();
        oldFiles_ << file;
    }

    duplicateFiles_.clear();
    emit modInfoChanged();
}

void LocalMod::rollback(LocalModFile *file)
{
    auto disabled = isDisabled();
    if(disabled) setEnabled(true);
    file->removeOld();
    oldFiles_.removeAll(file);
    modFile_->addOld();
    oldFiles_ << modFile_;
    setModFile(file);
    if(disabled) setEnabled(false);
    emit modInfoChanged();

    //reset update info
    curseforgeUpdater_.reset();
    modrinthUpdater_.reset();
//    connect(this, &LocalMod::websiteReady, this, [=]{
//        checkUpdates(false);
//    });
}

void LocalMod::deleteAllOld()
{
    for(auto &oldFile : qAsConst(oldFiles_))
        oldFile->remove();
    clearQObjects(oldFiles_);
    emit modInfoChanged();
}

void LocalMod::deleteOld(LocalModFile *file)
{
    if(file->remove()){
        oldFiles_.removeOne(file);
        file->setParent(nullptr);
        file->deleteLater();
    }
    emit modInfoChanged();
}

bool LocalMod::isDisabled()
{
    return modFile_->type() == LocalModFile::Disabled;
}

bool LocalMod::isEnabled()
{
    return !isDisabled();
}

bool LocalMod::setEnabled(bool enabled)
{
    auto bl = modFile_->setEnabled(enabled);
    if(bl){
        emit modInfoChanged();
        updateIcon();
    }
    return bl;
}

void LocalMod::clearIgnores()
{
    curseforgeUpdater_.clearIgnores();
    modrinthUpdater_.clearIgnores();
    emit modCacheUpdated();
    emit updateReady(updateTypes());
    checkUpdates();
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

QString LocalMod::displayName() const
{
    if(!alias_.isEmpty())
        return alias_;
    if(!commonInfo()->name().isEmpty())
        return commonInfo()->name();
    return modFile_->fileInfo().fileName();
}

const QList<LocalModFile *> &LocalMod::oldFiles() const
{
    return oldFiles_;
}

const QList<LocalModFile *> &LocalMod::duplicateFiles() const
{
    return duplicateFiles_;
}

const Updater<Curseforge> &LocalMod::curseforgeUpdate() const
{
    return curseforgeUpdater_;
}

const Updater<Modrinth> &LocalMod::modrinthUpdate() const
{
    return modrinthUpdater_;
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

void LocalMod::setCurseforgeId(int id, bool cache)
{
    if(id != 0){
        curseforgeMod_ = new CurseforgeMod(this, id);
        emit curseforgeReady(true);
        if(cache)
            IdMapper::addCurseforge(commonInfo()->id(), id);
        updateIcon();
    } else
        emit curseforgeReady(false);
}

void LocalMod::setModrinthId(const QString &id, bool cache)
{
    if(!id.isEmpty()){
        modrinthMod_ = new ModrinthMod(this, id);
        emit curseforgeReady(true);
        if(cache)
            IdMapper::addModrinth(commonInfo()->id(), id);
    } else
        emit modrinthReady(false);
}

const QString &LocalMod::alias() const
{
    return alias_;
}

void LocalMod::setAlias(const QString &newAlias)
{
    if(alias_ == newAlias) return;
    alias_ = newAlias;
    emit modInfoChanged();
    emit modCacheUpdated();
}

QJsonObject LocalMod::toJsonObject() const
{
    QJsonObject object;

    if(!alias_.isEmpty())
        object.insert("alias", alias_);
    if(isFeatured_)
        object.insert("featured", isFeatured_);
    if(auto taglist = tags(TagCategory::CustomizableCategories, true); !taglist.isEmpty()){
        QJsonArray tagArray;
        for(auto &&tag : taglist)
            tagArray << tag.toJsonValue();
        object.insert("tags", tagArray);
    }

    if(curseforgeMod_){
        QJsonObject curseforgeObject;
        curseforgeObject.insert("id", curseforgeMod_->modInfo().id());
        if(!curseforgeUpdater_.ignores().isEmpty()){
            QJsonArray ignoreArray;
            for(auto &&fileId : curseforgeUpdater_.ignores())
                ignoreArray << fileId;
            curseforgeObject.insert("ignored-updates", ignoreArray);
        }
        if(curseforgeUpdater_.updatableId())
            curseforgeObject.insert("current-file-id", curseforgeUpdater_.updatableId());
        if(!curseforgeUpdater_.updateFileInfos().isEmpty()){
            QJsonArray updateFileInfoArray;
            for(auto &&fileInfo : curseforgeUpdater_.updateFileInfos())
                updateFileInfoArray << fileInfo.toJsonObject();
            curseforgeObject.insert("available-updates", updateFileInfoArray);
        }
        object.insert("curseforge", curseforgeObject);
    }

    if(modrinthMod_){
        QJsonObject modrinthObject;
        modrinthObject.insert("id", modrinthMod_->modInfo().id());
        if(!modrinthUpdater_.ignores().isEmpty()){
            QJsonArray ignoreArray;
            for(auto &&fileId : modrinthUpdater_.ignores())
                ignoreArray << fileId;
            modrinthObject.insert("ignored-updates", ignoreArray);
        }
        if(!modrinthUpdater_.updatableId().isEmpty())
            modrinthObject.insert("current-file-id", curseforgeUpdater_.updatableId());
        if(!modrinthUpdater_.updateFileInfos().isEmpty()){
            QJsonArray updateFileInfoArray;
            for(auto &&fileInfo : modrinthUpdater_.updateFileInfos())
                updateFileInfoArray << fileInfo.toJsonObject();
            modrinthObject.insert("available-updates", updateFileInfoArray);
        }
        object.insert("modrinth", modrinthObject);
    }

    return object;
}

void LocalMod::restore(const QVariant &variant)
{
    alias_ = value(variant, "alias").toString();
    isFeatured_ = value(variant, "featured").toBool();
    for(auto &&tag : value(variant, "tags").toList())
        importTag(Tag::fromVariant(tag));
    if(contains(variant, "curseforge")){
        auto curseforgeVariant = value(variant, "curseforge");
        if(contains(curseforgeVariant, "id"))
            setCurseforgeId(value(curseforgeVariant, "id").toInt());
        if(contains(curseforgeVariant, "ignored-updates"))
            for(auto &&id : value(curseforgeVariant, "ignored-updates").toList())
                curseforgeUpdater_.addIgnore(id.toInt());
        if(contains(curseforgeVariant, "current-file-id")){
            auto id = value(curseforgeVariant, "current-file-id").toInt();
            if(auto fileInfo = modFile_->linker()->curseforgeFileInfo();
                    fileInfo && fileInfo->id() == id){
                curseforgeUpdater_.setUpdatableId(id);
                if(contains(curseforgeVariant, "available-updates"))
                    for(auto &&fileInfo : value(curseforgeVariant, "available-updates").toList())
                        curseforgeUpdater_ << CurseforgeFileInfo::fromVariant(fileInfo);
            }
        }
    }
    if(contains(variant, "modrinth")){
        auto modrinthVariant = value(variant, "modrinth");
        if(contains(modrinthVariant, "id"))
            setModrinthId(value(modrinthVariant, "id").toString());
        if(contains(value(variant, "modrinth"), "ignored-updates"))
            for(auto &&id : value(modrinthVariant, "ignored-updates").toList())
                modrinthUpdater_.addIgnore(id.toString());
        if(contains(modrinthVariant, "current-file-id")){
            auto id = value(modrinthVariant, "current-file-id").toString();
            if(auto fileInfo = modFile_->linker()->modrinthFileInfo();
                    fileInfo && fileInfo->id() == id){
                modrinthUpdater_.setUpdatableId(id);
                if(contains(modrinthVariant, "available-updates"))
                    for(auto &&fileInfo : value(modrinthVariant, "available-updates").toList())
                        modrinthUpdater_ << ModrinthFileInfo::fromVariant(fileInfo);
            }
        }
    }
    //restore from id mapper
    if(IdMapper::idMap().contains(commonInfo()->id())){
        auto id = IdMapper::idMap().value(commonInfo()->id());
        if(!curseforgeMod_ && id.curseforgeId())
            setCurseforgeId(id.curseforgeId(), false);
        if(!modrinthMod_ && !id.modrinthId().isEmpty())
            setModrinthId(id.modrinthId(), false);
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
    emit modInfoChanged();
}

LocalModPath *LocalMod::path() const
{
    return path_;
}

void LocalMod::setModFile(LocalModFile *newModFile)
{
    if(modFile_) disconnect(modFile_, &LocalModFile::fileChanged, this, &LocalMod::modInfoChanged);
    removeSubTagable(modFile_);
    modFile_ = newModFile;
    modFile_->setMod(this);
    addSubTagable(modFile_);
    if(modFile_) {
        connect(modFile_, &LocalModFile::fileChanged, this, &LocalMod::modInfoChanged);
        modFile_->setParent(this);
        if(modFile_->loaderType() == ModLoaderType::Fabric){
            if(auto environment = modFile_->fabric().environment(); environment == "*"){
                importTag(Tag::clientTag());
                importTag(Tag::serverTag());
            }else if(environment == "client")
                importTag(Tag::clientTag());
            else if(environment == "server")
                importTag(Tag::serverTag());
        }
    }
    updateIcon();
}

const QPixmap &LocalMod::icon() const
{
    return icon_;
}

void LocalMod::updateIcon()
{
    auto applyIcon = [=]{
        if(isDisabled()){
            QImage image(icon_.toImage());
            auto alphaChannel = image.convertToFormat(QImage::Format_Alpha8);
            image = image.convertToFormat(QImage::Format_Grayscale8);
            image.setAlphaChannel(alphaChannel);
            icon_.convertFromImage(image);
        }
        emit modIconUpdated();
    };

    if(!commonInfo()->iconBytes().isEmpty()){
        icon_.loadFromData(commonInfo()->iconBytes());
        applyIcon();
    }else if(commonInfo()->id() == "optifine") {
        icon_.load(":/image/optifine.png");
        applyIcon();
    }else if(curseforgeMod_){
        auto setCurseforgeIcon = [=]{
            icon_.loadFromData(curseforgeMod_->modInfo().iconBytes());
            applyIcon();
        };
        if(!curseforgeMod_->modInfo().iconBytes().isEmpty())
            setCurseforgeIcon();
        else{
            connect(curseforgeMod_, &CurseforgeMod::basicInfoReady, this, [=]{
                connect(curseforgeMod_, &CurseforgeMod::iconReady, this, setCurseforgeIcon);
                curseforgeMod_->acquireIcon();
            });
            curseforgeMod_->acquireBasicInfo();
        }
    } else if(modrinthMod_){
        auto setModrinthIcon = [=]{
            icon_.loadFromData(modrinthMod_->modInfo().iconBytes());
            applyIcon();
        };
        if(!modrinthMod_->modInfo().iconBytes().isEmpty())
            setModrinthIcon();
        else{
            modrinthMod_->acquireFullInfo();
            connect(modrinthMod_, &ModrinthMod::fullInfoReady, this, [=]{
                connect(modrinthMod_, &ModrinthMod::iconReady, this, setModrinthIcon);
                modrinthMod_->acquireIcon();
            });
        }
    } else{
        icon_.load(":/image/modmanager.png");
        applyIcon();
    }
}

ModrinthMod *LocalMod::modrinthMod() const
{
    return modrinthMod_;
}

void LocalMod::setModrinthMod(ModrinthMod *newModrinthMod)
{
    removeSubTagable(modrinthMod_);
    modrinthMod_ = newModrinthMod;
    addSubTagable(modrinthMod_);
    if(modrinthMod_){
        modrinthMod_->setParent(this);
        emit modrinthReady(true);
        emit modCacheUpdated();
        emit modInfoChanged();
    }
}
