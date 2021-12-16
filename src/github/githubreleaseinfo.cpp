#include "githubreleaseinfo.h"

#include "util/tutil.hpp"

GitHubReleaseInfo GitHubReleaseInfo::fromVariant(const QVariant &variant)
{
    GitHubReleaseInfo info;

    info.name_ = value(variant, "name").toString();
    info.tagName_ = value(variant, "tag_name").toString();
    info.url_ = value(variant, "url").toString();
    info.body_ = value(variant, "body").toString();
    info.prerelease_ = value(variant, "prerelease").toBool();

    for(auto &&variant : value(variant, "assets").toList())
        info.assets_ << GitHubFileInfo::fromVariant(variant);
    return info;
}

const QList<GitHubFileInfo> &GitHubReleaseInfo::assets() const
{
    return assets_;
}

const QString &GitHubReleaseInfo::name() const
{
    return name_;
}

const QString &GitHubReleaseInfo::tagName() const
{
    return tagName_;
}

const QString &GitHubReleaseInfo::body() const
{
    return body_;
}

const QUrl &GitHubReleaseInfo::url() const
{
    return url_;
}

bool GitHubReleaseInfo::prerelease() const
{
    return prerelease_;
}
