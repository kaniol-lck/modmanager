#include "downloadfileinfo.h"

DownloadFileInfo::DownloadFileInfo()
{

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
