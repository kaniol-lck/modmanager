#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QDir>
#include <QObject>
#include <optional>

#include "localmodfile.h"
#include "updatable.hpp"
#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"
#include "tag/localmodtags.h"
#include "modwebsitetype.h"

class LocalModPath;
class CurseforgeMod;
class ModrinthMod;
class CurseforgeAPI;
class ModrinthAPI;

class LocalMod : public QObject
{
    Q_OBJECT
public:
    explicit LocalMod(QObject *parent, LocalModFile *file);
    explicit LocalMod(LocalModPath *parent, LocalModFile *file);
    ~LocalMod();

    //info
    LocalModFile *modFile() const;
    const CommonModInfo *commonInfo() const;
    QString displayName() const;

    //link
    CurseforgeMod *curseforgeMod() const;
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    ModrinthMod *modrinthMod() const;
    void setModrinthMod(ModrinthMod *newModrinthMod);

    //search
    void searchOnWebsite();

    //update
    bool perpareUpdate();
    void checkUpdates(bool force = true);
    void cancelChecking();
    void checkCurseforgeUpdate(bool force = true);
    void checkModrinthUpdate(bool force = true);

    QAria2Downloader *downloadOldMod(DownloadFileInfo &info);

    ModWebsiteType defaultUpdateType() const;
    QList<ModWebsiteType> updateTypes() const;
    QAria2Downloader *update();
    template<typename FileInfoT>
    QAria2Downloader *update(const FileInfoT &fileInfo);

    template<typename FileInfoT>
    inline Updatable<FileInfoT> &updater();

    //api
    CurseforgeAPI *curseforgeAPI() const;
    ModrinthAPI *modrinthAPI() const;

    //old
    void addOldFile(LocalModFile *oldFile);
    const QList<LocalModFile *> &oldFiles() const;

    //duplicate
    void addDuplicateFile(LocalModFile *duplicateFile);
    const QList<LocalModFile *> &duplicateFiles() const;

    void duplicateToOld();
    void rollback(LocalModFile *file);
    void deleteAllOld();
    void deleteOld(LocalModFile *file);

    bool isDisabled();
    bool isEnabled();

    template<typename FileInfoT>
    void ignoreUpdate(const FileInfoT &fileInfo);
    void clearIgnores();

    void addDepend(std::tuple<QString, QString, std::optional<FabricModInfo>> modDepend);
    void addConflict(std::tuple<QString, QString, FabricModInfo> modConflict);
    void addBreak(std::tuple<QString, QString, FabricModInfo> modBreak);

    const Updatable<CurseforgeFileInfo> &curseforgeUpdate() const;
    const Updatable<ModrinthFileInfo> &modrinthUpdate() const;
    void setCurrentCurseforgeFileInfo(const std::optional<CurseforgeFileInfo> &info, bool cache = true);
    void setCurrentModrinthFileInfo(const std::optional<ModrinthFileInfo> &info, bool cache = true);
    const QList<std::tuple<QString, QString, std::optional<FabricModInfo> > > &depends() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &conflicts() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &breaks() const;

    void setCurseforgeId(int id, bool cache = true);
    void setModrinthId(const QString &id, bool cache = true);

    const QString &alias() const;
    void setAlias(const QString &newAlias);

    QJsonObject toJsonObject() const;
    void restore(const QVariant &variant);

    bool isFeatured() const;

    LocalModPath *path() const;

    const QList<Tag> tags() const;
    const QList<Tag> customizableTags() const;
    void addTag(const Tag &tag);
    void removeTag(const Tag &tag);
    const LocalModTags &tagManager() const;

    void setModFile(LocalModFile *newModFile);

    const QPixmap &icon() const;

public slots:
    bool setEnabled(bool enabled);
    void setFeatured(bool featured);

signals:
    void modFileUpdated();
    void modIconUpdated();
    void modCacheUpdated();

    void websiteReady(bool bl);
    void checkUpdatesStarted();
    void checkCancelled();
    void updateReady(QList<ModWebsiteType> types, bool success = true);

    void curseforgeReady(bool bl);
    void curseforgeUpdateReady(bool hasUpdate, bool success = true);
    void modrinthReady(bool bl);
    void modrinthUpdateReady(bool hasUpdate, bool success = true);

    void checkCurseforgeStarted();
    void checkCurseforgeUpdateStarted();
    void checkModrinthStarted();
    void checkModrinthUpdateStarted();

    void updateStarted();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateFinished(bool success);

private:
    bool isFeatured_ = false;
    QString alias_;
    LocalModTags tagManager_;
    QPixmap icon_;

    //api for update
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;

    //files
    LocalModPath *path_ = nullptr;
    LocalModFile *modFile_ = nullptr;
    QList<LocalModFile*> oldFiles_;
    QList<LocalModFile*> duplicateFiles_;

    //target for update
    GameVersion targetVersion_;
    ModLoaderType::Type targetLoaderType_;

    //related on websites
    CurseforgeMod *curseforgeMod_ = nullptr;
    Updatable<CurseforgeFileInfo> curseforgeUpdate_;

    ModrinthMod *modrinthMod_ = nullptr;
    Updatable<ModrinthFileInfo> modrinthUpdate_;

    //dependencies
    QList<std::tuple<QString, QString, std::optional<FabricModInfo>>> depends_;
    QList<std::tuple<QString, QString, FabricModInfo>> conflicts_;
    QList<std::tuple<QString, QString, FabricModInfo>> breaks_;

    void updateIcon();
};

template<typename FileInfoT>
inline void LocalMod::ignoreUpdate(const FileInfoT &fileInfo)
{
    updater<FileInfoT>().addIgnore(fileInfo);
    emit updateReady(updateTypes());
    emit modCacheUpdated();
}

template<>
inline Updatable<CurseforgeFileInfo> &LocalMod::updater(){
    return curseforgeUpdate_;
}

template<>
inline Updatable<ModrinthFileInfo> &LocalMod::updater(){
    return modrinthUpdate_;
}

template<typename FileInfoT>
QAria2Downloader *LocalMod::update(const FileInfoT &fileInfo)
{
    emit updateStarted();

    auto path = QFileInfo(modFile_->path()).absolutePath();
    auto newPath = QDir(path).absoluteFilePath(fileInfo.fileName());
    auto file = new LocalModFile(this, newPath);
    auto callback1 = [=]{
        file->loadInfo();
        //loader type mismatch
        if(!file->loaderTypes().contains(modFile_->loaderType())){
            emit updateFinished(false);
            return false;
        }

        //deal with old mod file
        auto postUpdate = Config().getPostUpdate();
        if(postUpdate == Config::Delete){
            //remove old file
            if(!modFile_->remove())
                return false;
            modFile_->deleteLater();
        } else if(postUpdate == Config::Keep){
            if(!modFile_->addOld())
                return false;
            oldFiles_ << modFile_;
        }

        //        //TODO: if version updated from curseforge also in modrinth, etc...
        //        if(type == Curseforge)
        //            modrinthUpdate_.reset(true);
        //        if(type == Modrinth)
        //            curseforgeUpdate_.reset(true);

        setModFile(file);
        emit modFileUpdated();

        return true;
    };
    auto callback2 = [=]{
        emit modCacheUpdated();
        emit updateFinished(true);
    };

    QAria2Downloader *downloader;

    downloader = updater<FileInfoT>().update(path, modFile_->commonInfo()->iconBytes(), fileInfo, callback1, callback2);
    connect(downloader, &AbstractDownloader::downloadProgress, this, &LocalMod::updateProgress);
    return downloader;
}

#endif // LOCALMOD_H
