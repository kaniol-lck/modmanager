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

    QPair<QString, QString> updateNames() const
    {
        return QPair(currentFileInfo_->displayName(), updateFileInfo_->displayName());
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
        return true;
    }

    ModDownloader *update(const QString &path, const QByteArray &iconBytes, std::function<bool (FileInfoT)> callback1, std::function<void ()> callback2)
    {
        DownloadFileInfo info(*updateFileInfo_);
        info.setPath(path);
        info.setIconBytes(iconBytes);
        return DownloadManager::addModUpdate(info, [=]{
            if(callback1(*updateFileInfo_)){
                currentFileInfo_.emplace(*updateFileInfo_);
                updateFileInfo_.reset();
                callback2();
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

private:
    std::optional<FileInfoT> currentFileInfo_;
    std::optional<FileInfoT> updateFileInfo_;
};

#endif // UPDATABLE_HPP
