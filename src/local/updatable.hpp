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

    qint64 size() const
    {
        return updateFileInfo_->size();
    }

    QDateTime fileDate() const
    {
        return updateFileInfo_->fileDate();
    }

    std::optional<FileInfoT> currentFileInfo() const
    {
        return currentFileInfo_;
    }

    std::optional<FileInfoT> updateFileInfo() const
    {
        return updateFileInfo_;
    }

    QPair<QString, QString> updateNames() const
    {
        return QPair(currentFileInfo_? currentFileInfo_->displayName() : "",
                     updateFileInfo_? updateFileInfo_->displayName() : "");
    }

    QPair<QString, QString> updateInfos() const
    {
        auto getInfo = [=](const auto &info){
            if(!info) return QString();
            return QStringList{
                        info->displayName(),
                        info->fileName(),
                        info->fileDate().toString()
            }.join("\n");
        };
        return QPair(getInfo(currentFileInfo_), getInfo(updateFileInfo_));
    }

    void addIgnore(){
        if(!updateFileInfo_) return;
        auto id = updateFileInfo_->id();
        updateFileInfo_.reset();
        ignores_ << id;
    }

    void addIgnore(const typename FileInfoT::IdType &id){
        ignores_ << id;
        if(updateFileInfo_)
            updateFileInfo_.reset();
    }

    void clearIgnores(){
        ignores_.clear();
    }

    void reset(bool clearCurrent = false){
        updateFileInfo_.reset();
        if(clearCurrent)
            currentFileInfo_.reset();
    }

    bool findUpdate(QList<FileInfoT> fileList, const GameVersion &targetVersion, ModLoaderType::Type targetType)
    {
        //select mod file for matched game versions and mod loader type
        QList<FileInfoT> list;
        std::insert_iterator<QList<FileInfoT>> iter(list, list.begin());
        std::copy_if(fileList.cbegin(), fileList.cend(), iter, [=](const auto &file){
            if(ignores_.contains(file.id()))
                return false;
            bool versionCheck = false;
            Config config;
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
            bool loaderCheck = false;
            if(file.loaderTypes().contains(targetType))
                loaderCheck = true;
            if(!loaderCheck && config.getLoaderMatch() == Config::IncludeUnmarked && file.loaderTypes().isEmpty())
                loaderCheck = true;

            return versionCheck && loaderCheck;
        });

        //non match
        if(list.isEmpty()) return false;

        //find latesest file
        auto resultIter = std::max_element(list.cbegin(), list.cend(), [=](const auto &file1, const auto &file2){
            return file1.fileDate() < file2.fileDate();
        });

        //not older or same version
        if(resultIter->fileDate() <= currentFileInfo_->fileDate())
            return false;

        if(resultIter->id() == currentFileInfo_->id())
            return false;

        updateFileInfo_.emplace(*resultIter);
        return true;
    }

    QAria2Downloader *update(const QString &path, const QByteArray &iconBytes, std::function<bool (FileInfoT)> callback1, std::function<void ()> callback2)
    {
        DownloadFileInfo info(*updateFileInfo_);
        info.setPath(path);
        info.setIconBytes(iconBytes);
        auto downloader = DownloadManager::manager()->download(info);
        QObject::connect(downloader, &QAria2Downloader::finished, [=]{
            if(callback1(*updateFileInfo_)){
                currentFileInfo_.emplace(*updateFileInfo_);
                updateFileInfo_.reset();
                callback2();
            }
        });
        return downloader;
    }

    void setCurrentFileInfo(FileInfoT newCurrentFileInfo)
    {
        currentFileInfo_.emplace(newCurrentFileInfo);
    }

    void setUpdateFileInfo(FileInfoT newUpdateFileInfo)
    {
        updateFileInfo_.emplace(newUpdateFileInfo);
    }

    const QList<typename FileInfoT::IdType> &ignores() const
    {
        return ignores_;
    }

private:
    std::optional<FileInfoT> currentFileInfo_;
    std::optional<FileInfoT> updateFileInfo_;
    QList<typename FileInfoT::IdType> ignores_;
};



#endif // UPDATABLE_HPP
