#include "githubfileinfo.h"

#include <QDebug>

#include "util/tutil.hpp"

GitHubFileInfo GitHubFileInfo::fromVariant(const QVariant &variant)
{
    GitHubFileInfo info;
    info.name_ = value(variant, "name").toString();
    info.size_ = value(variant, "size").toLongLong();
    info.url_ = value(variant, "url").toUrl();
    info.downloadUrl_ = value(variant, "browser_download_url").toUrl();
    return info;
}

const QString &GitHubFileInfo::name() const
{
    return name_;
}

qint64 GitHubFileInfo::size() const
{
    return size_;
}

const QUrl &GitHubFileInfo::url() const
{
    return url_;
}

const QUrl &GitHubFileInfo::downloadUrl() const
{
    return downloadUrl_;
}
