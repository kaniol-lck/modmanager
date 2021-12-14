#ifndef LOCALMOD_H
#define LOCALMOD_H

#include <QDir>
#include <QObject>
#include <optional>

#include "localmodfile.h"
#include "localfilelinker.h"
#include "updater.hpp"
#include "curseforge/curseforgefileinfo.h"
#include "modrinth/modrinthfileinfo.h"
#include "tag/tagable.h"
#include "modwebsitetype.h"

class CheckSheet;
class LocalModPath;
class CurseforgeMod;
class ModrinthMod;
class CurseforgeAPI;
class ModrinthAPI;

class LocalMod : public QObject, public Tagable
{
    Q_OBJECT
public:
//    explicit LocalMod(QObject *parent, LocalModFile *file);
    explicit LocalMod(LocalModPath *parent, LocalModFile *file);
    ~LocalMod();

    //info
    const CommonModInfo *commonInfo() const;
    QString displayName() const;
    bool isFeatured() const;
    bool isDisabled();
    bool isEnabled();
    const QPixmap &icon() const;

    //path
    LocalModPath *path() const;

    //link
    //mod basic info start to request after set
    CurseforgeMod *curseforgeMod() const;
    void setCurseforgeId(int id, bool cache = true);
    void setCurseforgeMod(CurseforgeMod *newCurseforgeMod);

    ModrinthMod *modrinthMod() const;
    void setModrinthMod(ModrinthMod *newModrinthMod);
    void setModrinthId(const QString &id, bool cache = true);

    //check update
    //request new file list
    void checkUpdates(bool force = true);
    void cancelChecking();
    void checkCurseforgeUpdate(bool force = true);
    void checkModrinthUpdate(bool force = true);

    ModWebsiteType defaultUpdateType() const;
    QList<ModWebsiteType> updateTypes() const;

    //update
    QAria2Downloader *update();
    template<typename FileInfoT>
    QAria2Downloader *update(const FileInfoT &fileInfo);

    //updater
    template<ModWebsiteType type>
    inline Updater<type> &updater();
    const Updater<Curseforge> &curseforgeUpdater() const;
    const Updater<Modrinth> &modrinthUpdater() const;

    //update ignores
    template<typename FileInfoT>
    void ignoreUpdate(const FileInfoT &fileInfo);
    void clearIgnores();

    //download
    QAria2Downloader *downloadOldMod(DownloadFileInfo &info);

    //api
    CurseforgeAPI *curseforgeAPI() const;
    ModrinthAPI *modrinthAPI() const;

    //current file
    LocalModFile *modFile() const;
    void setModFile(LocalModFile *newModFile);

    QList<LocalModFile *> files() const;

    //old files
    void addOldFile(LocalModFile *oldFile);
    const QList<LocalModFile *> &oldFiles() const;

    //duplicate files
    void addDuplicateFile(LocalModFile *duplicateFile);
    const QList<LocalModFile *> &duplicateFiles() const;

    //file operations
    void duplicateToOld();
    void rollback(LocalModFile *file);
    void deleteAllOld();
    void deleteOld(LocalModFile *file);

    const QString &alias() const;
    void setAlias(const QString &newAlias);

    QJsonObject toJsonObject() const;
    void restore(const QVariant &variant);

    void addDepend(std::tuple<QString, QString, std::optional<FabricModInfo>> modDepend);
    void addConflict(std::tuple<QString, QString, FabricModInfo> modConflict);
    void addBreak(std::tuple<QString, QString, FabricModInfo> modBreak);

    const QList<std::tuple<QString, QString, std::optional<FabricModInfo> > > &depends() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &conflicts() const;
    const QList<std::tuple<QString, QString, FabricModInfo> > &breaks() const;

    CheckSheet *updateChecker() const;

public slots:
    bool setEnabled(bool enabled);
    void setFeatured(bool featured);

    void setCurseforgeFileInfos(const QList<CurseforgeFileInfo> &fileInfos);
    void setModrinthFileInfos(const QList<ModrinthFileInfo> &fileInfos);

signals:
    //tags
    void tagsChanged() override;
    void tagsEditted() override;

    void modInfoChanged();
    void modIconUpdated();
    void modCacheUpdated();

    //link
    void curseforgeReady(bool bl);
    void curseforgeUpdateReady(bool hasUpdate, bool success = true);
    void modrinthReady(bool bl);
    void modrinthUpdateReady(bool hasUpdate, bool success = true);

    //update
//    void checkUpdatesStarted();
//    void checkCancelled();
    void updateReady();

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
    QPixmap icon_;

    //api for update
    CurseforgeAPI *curseforgeAPI_;
    ModrinthAPI *modrinthAPI_;

    //files
    LocalModPath *path_ = nullptr;
    LocalModFile *modFile_ = nullptr;
    QList<LocalModFile*> oldFiles_;
    QList<LocalModFile*> duplicateFiles_;

    //related on websites
    CurseforgeMod *curseforgeMod_ = nullptr;
    ModrinthMod *modrinthMod_ = nullptr;

    //updater
    Updater<Curseforge> curseforgeUpdater_;
    Updater<Modrinth> modrinthUpdater_;

    CheckSheet *updateChecker_;

    //dependencies
    QList<std::tuple<QString, QString, std::optional<FabricModInfo>>> depends_;
    QList<std::tuple<QString, QString, FabricModInfo>> conflicts_;
    QList<std::tuple<QString, QString, FabricModInfo>> breaks_;

    void updateIcon();
};

template<typename FileInfoT>
inline void LocalMod::ignoreUpdate(const FileInfoT &fileInfo)
{
    updater<FileInfoT::Type>().addIgnore(fileInfo);
    emit updateReady();
    emit modCacheUpdated();
}

template<>
inline Updater<Curseforge> &LocalMod::updater(){
    return curseforgeUpdater_;
}

template<>
inline Updater<Modrinth> &LocalMod::updater(){
    return modrinthUpdater_;
}

template<typename FileInfoT>
QAria2Downloader *LocalMod::update(const FileInfoT &fileInfo)
{
    emit updateStarted();

    auto path = QFileInfo(modFile_->path()).absolutePath();
    auto newPath = QDir(path).absoluteFilePath(fileInfo.fileName());
    auto file = new LocalModFile(path_, newPath);
    auto callback1 = [=]{
        file->loadInfo();
        file->linker()->linkCached();
        //loader type mismatch
        if(!file->loaderTypes().contains(modFile_->loaderType())){
            emit updateFinished(false);
            return false;
        }

        bool isDisabled = modFile_->type() == LocalModFile::Disabled;

        //deal with old mod file
        auto postUpdate = Config().getPostUpdate();
        if(postUpdate == Config::Delete){
            //remove old file
            if(!modFile_->remove())
                return false;
            modFile_->deleteLater();
        } else if(postUpdate == Config::Keep){
            if(isDisabled)
                modFile_->setEnabled(true);
            if(!modFile_->addOld())
                return false;
            oldFiles_ << modFile_;
        }

        modrinthUpdater_.reset();
        curseforgeUpdater_.reset();

        setModFile(file);
        if(isDisabled)
            file->setEnabled(false);
        emit modInfoChanged();

        return true;
    };
    auto callback2 = [=]{
        emit modCacheUpdated();
        emit updateFinished(true);
    };

    auto downloader = updater<FileInfoT::Type>().update(path, modFile_->commonInfo()->iconBytes(), fileInfo, callback1, callback2);
    connect(downloader, &AbstractDownloader::downloadProgress, this, &LocalMod::updateProgress);
    return downloader;
}

#endif // LOCALMOD_H
