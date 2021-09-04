#include "downloadfileinfo.h"

DownloadFileInfo::DownloadFileInfo()
{

}

const QString &DownloadFileInfo::getDisplayName() const
{
    return displayName;
}

const QString &DownloadFileInfo::getFileName() const
{
    return fileName;
}

const QUrl &DownloadFileInfo::getUrl() const
{
    return url;
}

qint64 DownloadFileInfo::getSize() const
{
    return size;
}
