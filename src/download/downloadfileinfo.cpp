#include "downloadfileinfo.h"

DownloadFileInfo::DownloadFileInfo(const QUrl &url) :
    url_(url)
{}

DownloadFileInfo::DownloadFileInfo(const CurseforgeFileInfo &info) :
    DownloadFileInfo(info.url())
{
    displayName_ = info.displayName();
    fileName_ = info.fileName();
    size_ = info.size();
}

DownloadFileInfo::DownloadFileInfo(const ModrinthFileInfo &info) :
    DownloadFileInfo(info.url())
{
    displayName_ = info.displayName();
    fileName_ = info.fileName();
    //modrinth does not provide size info
}

DownloadFileInfo::DownloadFileInfo(const OptifineModInfo &info) :
    DownloadFileInfo(info.downloadUrl())
{
    displayName_ = info.name();
    fileName_ = info.fileName();
    //optifine does not provide size info
}

DownloadFileInfo DownloadFileInfo::fromCurseforge(const CurseforgeFileInfo &info)
{
    DownloadFileInfo downloadInfo(info.url());
    downloadInfo.displayName_ = info.displayName();
    downloadInfo.fileName_ = info.fileName();
    downloadInfo.size_ = info.size();
    return downloadInfo;
}

DownloadFileInfo DownloadFileInfo::fromModrinth(const ModrinthFileInfo &info)
{
    DownloadFileInfo downloadInfo(info.url());
    downloadInfo.displayName_ = info.displayName();
    downloadInfo.fileName_ = info.fileName();
    //modrinth does not provide size info
    return downloadInfo;
}

const QString &DownloadFileInfo::displayName() const
{
    return displayName_;
}

const QString &DownloadFileInfo::fileName() const
{
    return fileName_;
}

const QUrl &DownloadFileInfo::url() const
{
    return url_;
}

qint64 DownloadFileInfo::size() const
{
    return size_;
}

const QString &DownloadFileInfo::path() const
{
    return path_;
}

void DownloadFileInfo::setPath(const QString &newPath)
{
    path_ = newPath;
}

const QByteArray &DownloadFileInfo::iconBytes() const
{
    return iconBytes_;
}

void DownloadFileInfo::setIconBytes(const QByteArray &newIconBytes)
{
    iconBytes_ = newIconBytes;
}
