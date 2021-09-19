#ifndef UPDATABLE_HPP
#define UPDATABLE_HPP

#include <QDateTime>
#include <optional>
#include <QDebug>

#include "download/downloadmanager.h"
#include "config.h"
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

    std::optional<typename FileInfoT::IdType> fileId() const
    {
        return fileId_;
    }

    std::optional<typename FileInfoT::IdType> updateFileId() const
    {
        return updateFileId_;
    }

    QPair<QString, QString> updateNames() const
    {
        return QPair(currentFileInfo_->displayName(), updateFileInfo_->displayName());
    }

    void reset(bool clearCurrent = false){
        updateFileInfo_.reset();
        updateFileId_.reset();
        if(clearCurrent){
            currentFileInfo_.reset();
            fileId_.reset();
        }
    }

    void perpareFileInfo(QList<FileInfoT> fileList){
        if(!currentFileInfo_){
            if(auto it = std::find_if(fileList.cbegin(), fileList.cend(), [=](const auto &fileInfo){
                return fileInfo.id() == fileId_;
            }); it != fileList.cend())
                currentFileInfo_.emplace(*it);
            else
                fileId_.reset();
        }
        if(!updateFileInfo_){
            if(auto it = std::find_if(fileList.cbegin(), fileList.cend(), [=](const auto &fileInfo){
                return fileInfo.id() == updateFileId_;
            }); it != fileList.cend())
                updateFileInfo_.emplace(*it);
            else
                updateFileId_.reset();
        }
    }

    bool findUpdate(QList<FileInfoT> fileList, const GameVersion &targetVersion, ModLoaderType::Type targetType)
    {
        if(!currentFileInfo_){
            if(auto it = std::find_if(fileList.cbegin(), fileList.cend(), [=](const auto &fileInfo){
                return fileInfo.id() == fileId_;
            }); it != fileList.cend())
                currentFileInfo_.emplace(*it);
            else
                return false;
        }

        //select mod file for matched game versions and mod loader type
        QList<FileInfoT> list;
        std::insert_iterator<QList<FileInfoT>> iter(list, list.begin());
        std::copy_if(fileList.cbegin(), fileList.cend(), iter, [=](const auto &file){
            auto versionCheck = false;
            for(const auto & version : file.gameVersions()){
                switch (Config().getVersionMatch()) {
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
            return versionCheck && (file.loaderTypes().isEmpty() || file.loaderTypes().contains(targetType));
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

        updateFileInfo_.emplace(*resultIter);
        updateFileId_.emplace(resultIter->id());
        return true;
    }

    ModDownloader *update(const QString &path, const QByteArray &iconBytes, std::function<bool (FileInfoT)> callback, QList<FileInfoT> fileList = {})
    {
        if(!updateFileInfo_){
            if(auto it = std::find_if(fileList.cbegin(), fileList.cend(), [=](const auto &fileInfo){
                return fileInfo.id() == updateFileId_;
            }); it != fileList.cend())
                updateFileInfo_.emplace(*it);
            else
                return nullptr;
        }
        DownloadFileInfo info(*updateFileInfo_);
        info.setPath(path);
        info.setIconBytes(iconBytes);
        return DownloadManager::addModUpdate(info, [=]{
            if(callback(*updateFileInfo_)){
                currentFileInfo_.emplace(*updateFileInfo_);
                updateFileInfo_.reset();
                updateFileId_.reset();
                fileId_ = currentFileInfo_->id();
            }
        });
    }

    void setCurrentFileInfo(FileInfoT newCurrentFileInfo)
    {
        currentFileInfo_.emplace(newCurrentFileInfo);
    }

    void setUpdateFileInfo(FileInfoT newUpdateFileInfo)
    {
        updateFileInfo_.emplace(newUpdateFileInfo);
    }

    void setFileId(const typename FileInfoT::IdType &newFileId){
        fileId_.emplace(newFileId);
    }

    void setUpdateFileId(const typename FileInfoT::IdType &newUpdateFileId){
        updateFileId_.emplace(newUpdateFileId);
    }

private:
    std::optional<FileInfoT> currentFileInfo_;
    std::optional<FileInfoT> updateFileInfo_;
    std::optional<typename FileInfoT::IdType> fileId_;
    std::optional<typename FileInfoT::IdType> updateFileId_;
};

#endif // UPDATABLE_HPP
