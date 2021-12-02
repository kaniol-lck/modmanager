#ifndef UPDATABLE_HPP
#define UPDATABLE_HPP

#include <QDateTime>
#include <optional>
#include <QDebug>

#include "download/downloadmanager.h"
#include "download/qaria2downloader.h"
#include "config.hpp"
#include "gameversion.h"
#include "modloadertype.h"

template<typename FileInfoT>
class Updatable
{
public:
    Updatable() = default;

    std::optional<FileInfoT> currentFileInfo() const
    {
        return currentFileInfo_;
    }

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

    void clearIgnores(){
        ignores_.clear();
    }

    void reset(bool clearCurrent = false){
        updateFileInfos_.clear();
        if(clearCurrent)
            currentFileInfo_.reset();
    }

    bool findUpdate(const QList<FileInfoT> fileList, const GameVersion &targetVersion, ModLoaderType::Type targetType)
    {
        //select mod file for matched game versions and mod loader type
        updateFileInfos_.clear();
        std::insert_iterator<QList<FileInfoT>> iter(updateFileInfos_, updateFileInfos_.begin());
        std::copy_if(fileList.cbegin(), fileList.cend(), iter, [=](const auto &file){
            Config config;
            if(ignores_.contains(file.id()))
                return false;
            //check release type
            bool releaseTypeCheck = false;
            if(file.releaseType() == FileInfoT::Release)
                releaseTypeCheck = true;
            if(config.getUseBetaUpdate() && file.releaseType() == FileInfoT::Beta)
                releaseTypeCheck = true;
            if(config.getUseAlphaUpdate() && file.releaseType() == FileInfoT::Beta)
                releaseTypeCheck = true;
            if(!releaseTypeCheck) return false;
            //check version
            bool versionCheck = false;
            for(auto &&version : file.gameVersions()){
                switch (config.getVersionMatch()) {
                case Config::MinorVersion:
                    if(version == targetVersion)
                        versionCheck = true;
                    break;
                case Config::MajorVersion:
                    if(version.majorVersion() == targetVersion.majorVersion())
                        versionCheck = true;
                    break;
                }
            }
            if(!versionCheck) return false;
            //check loader type
            bool loaderCheck = false;
            if(file.loaderTypes().contains(targetType))
                loaderCheck = true;
            if(!loaderCheck && config.getLoaderMatch() == Config::IncludeUnmarked && file.loaderTypes().isEmpty())
                loaderCheck = true;
            if(!loaderCheck) return false;
            //not older or same version
            if(file.fileDate() <= currentFileInfo_->fileDate()) return false;
            if(file.id() == currentFileInfo_->id()) return false;
            //all pass
            return true;
        });

        //non match
        if(updateFileInfos_.isEmpty()) return false;

        //sort in filedate
        std::sort(updateFileInfos_.begin(), updateFileInfos_.end(), [=](const auto &file1, const auto &file2){
            return file1.fileDate() > file2.fileDate();
        });

        //all pass
        return true;
    }

    QAria2Downloader *update(const QString &path, const QByteArray &iconBytes, const FileInfoT &fileInfo , std::function<bool ()> callback1, std::function<void ()> callback2)
    {
        DownloadFileInfo info(fileInfo);
        info.setPath(path);
        info.setIconBytes(iconBytes);
        auto downloader = DownloadManager::manager()->download(info);
        QObject::connect(downloader, &QAria2Downloader::finished, [=]{
            if(callback1()){
                setCurrentFileInfo(fileInfo);
                if(auto index = updateFileInfos_.indexOf(fileInfo); index >= 0)
                    while(index != updateFileInfos_.size())
                        updateFileInfos_.removeAt(index);
                callback2();
            }
        });
        return downloader;
    }

    void setCurrentFileInfo(std::optional<FileInfoT> newCurrentFileInfo)
    {
        currentFileInfo_ = newCurrentFileInfo;
    }

    void setCurrentFileInfo(FileInfoT newCurrentFileInfo)
    {
        currentFileInfo_.emplace(newCurrentFileInfo);
    }

    const QList<typename FileInfoT::IdType> &ignores() const
    {
        return ignores_;
    }

    bool hasUpdate() const
    {
        return !updateFileInfos_.isEmpty();
    }

private:
    std::optional<FileInfoT> currentFileInfo_;
    QList<FileInfoT> updateFileInfos_;
    QList<typename FileInfoT::IdType> ignores_;
};

#endif // UPDATABLE_HPP
