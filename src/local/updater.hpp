#ifndef UPDATER_H
#define UPDATER_H

#include <QDateTime>
#include <optional>
#include <QDebug>

#include "download/downloadmanager.h"
#include "download/qaria2downloader.h"
#include "config.hpp"
#include "gameversion.h"
#include "modloadertype.h"
#include "modwebsitetype.h"
template<ModWebsiteType type>
class Updater
{
    friend class LocalModFile;
    using FileInfoT = typename CommonClass<type>::FileInfo;
public:
//    Updater() = default;
    Updater(const GameVersion &targetVersion, ModLoaderType::Type targetType) :
        targetVersion_(targetVersion),
        targetType_(targetType)
    {}

    const QList<FileInfoT> &updateFileInfos() const
    {
        return updateFileInfos_;
    }

    void addIgnore(const typename FileInfoT::IdType &id){
        addIgnore(FileInfoT(id));
    }

    void addIgnore(const FileInfoT& fileInfo){
        updateFileInfos_.removeAll(fileInfo);
        ignores_ << fileInfo.id();
    }

    Updater &operator<<(const FileInfoT &fileInfo){
        updateFileInfos_ << fileInfo;
        return *this;
    }

    void clearIgnores(){
        ignores_.clear();
    }

    void reset(){
        updateFileInfos_.clear();
        updatableId_ = typename CommonClass<type>::Id();
    }

    bool findUpdate(const std::optional<FileInfoT> &currentFileInfo)
    {
        //select mod file for matched game versions and mod loader type
        updateFileInfos_.clear();
        updatableId_ = typename CommonClass<type>::Id();
        std::insert_iterator<QList<FileInfoT>> iter(updateFileInfos_, updateFileInfos_.begin());
        std::copy_if(availableFileInfos_.cbegin(), availableFileInfos_.cend(), iter, [=](const auto &file){
            Config config;
            if(ignores_.contains(file.id()))
                return false;
            //check release type
            bool releaseTypeCheck = false;
            if(file.releaseType() == FileInfoT::Release)
                releaseTypeCheck = true;
            if(config.getUseBetaUpdate() && file.releaseType() == FileInfoT::Beta)
                releaseTypeCheck = true;
            if(config.getUseAlphaUpdate() && file.releaseType() == FileInfoT::Alpha)
                releaseTypeCheck = true;
            if(!releaseTypeCheck) return false;
            //check version
            bool versionCheck = false;
            for(auto &&version : file.gameVersions()){
                switch (config.getVersionMatch()) {
                case Config::MinorVersion:
                    if(version == targetVersion_)
                        versionCheck = true;
                    break;
                case Config::MajorVersion:
                    if(version.majorVersion() == targetVersion_.majorVersion())
                        versionCheck = true;
                    break;
                }
            }
            if(!versionCheck) return false;
            //check loader type
            bool loaderCheck = false;
            if(file.loaderTypes().contains(targetType_))
                loaderCheck = true;
            if(!loaderCheck && config.getLoaderMatch() == Config::IncludeUnmarked && file.loaderTypes().isEmpty())
                loaderCheck = true;
            if(!loaderCheck) return false;
            //not older or same version
            if(file.fileDate() <= currentFileInfo->fileDate()) return false;
            if(file.id() == currentFileInfo->id()) return false;
            //all pass
            return true;
        });

        //non match
        if(updateFileInfos_.isEmpty()) return false;

        //sort in filedate
        std::sort(updateFileInfos_.begin(), updateFileInfos_.end(), [=](const auto &file1, const auto &file2){
            return file1.fileDate() > file2.fileDate();
        });

        updatableId_ = currentFileInfo->id();

        //all pass
        return true;
    }

    // context 必须传：downloader 挂在（只增不减的）DownloadManager::downloaders_ 上，
    // 比 LocalMod 活得久。没有 context 的话，更新下载期间 reload（loadMods 会 qDeleteAll(modMap_)）
    // 之后回调仍会在已析构的 LocalMod / Updater 上跑。
    // 带 context 时 Qt 会在 context 析构时自动断开，回调不再触发。
    QAria2Downloader *update(const QString &path, const QByteArray &iconBytes, const FileInfoT &fileInfo,
                            std::function<bool ()> callback1, std::function<void ()> callback2,
                            std::function<void ()> callbackFail, QObject *context)
    {
        DownloadFileInfo info(fileInfo);
        info.setPath(path);
        info.setIconBytes(iconBytes);
        auto downloader = DownloadManager::manager()->download(info);
        QObject::connect(downloader, &QAria2Downloader::finished, context, [=]{
            if(callback1()){
                updateFileInfos_.clear();
                updatableId_ = typename CommonClass<type>::Id();
                callback2();
            }
        });
        // 下载失败也要报告一次。aria2 出错时只发 downloadFailed 不发 finished，
        // 少了这一条，上层（LocalModPath::updateMods 的计数）会永远差一条，
        // isUpdating_ 卡在 true，进度条常驻、Update All 不再恢复。
        QObject::connect(downloader, &QAria2Downloader::downloadFailed, context, [=]{
            callbackFail();
        });
        return downloader;
    }

    const QList<typename FileInfoT::IdType> &ignores() const
    {
        return ignores_;
    }

    bool hasUpdate() const
    {
        return !updateFileInfos_.isEmpty();
    }

    void setMod(typename CommonClass<type>::Mod *newMod)
    {
        mod_ = newMod;
    }

    const typename CommonClass<type>::Id &updatableId() const
    {
        return updatableId_;
    }

    void setUpdatableId(const typename CommonClass<type>::Id &newUpdatableId)
    {
        updatableId_ = newUpdatableId;
    }

    void setAvailableFileInfos(const QList<FileInfoT> &newAvailableFileInfos)
    {
        availableFileInfos_ = newAvailableFileInfos;
    }

    void setTargetVersion(const GameVersion &newTargetVersion)
    {
        targetVersion_ = newTargetVersion;
    }

    void setTargetType(ModLoaderType::Type newTargetType)
    {
        targetType_ = newTargetType;
    }

private:
    typename CommonClass<type>::Mod *mod_;
    typename CommonClass<type>::Id updatableId_;
    QList<FileInfoT> updateFileInfos_;
    QList<FileInfoT> availableFileInfos_;
    QList<typename FileInfoT::IdType> ignores_;

    GameVersion targetVersion_;
    ModLoaderType::Type targetType_;
};

#endif // UPDATER_H
