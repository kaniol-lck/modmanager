#include "githubrepoinfo.h"

GitHubRepoInfo::GitHubRepoInfo(const QString &user, const QString &repo, const QString &name, const QIcon &icon) :
    name_(name),
    user_(user),
    repo_(repo),
    icon_(icon)
{}

QUrl GitHubRepoInfo::url() const
{
    return QString("https://github.com/%1/%2").arg(user_, repo_);
}

const QString &GitHubRepoInfo::name() const
{
    return name_.isEmpty()? repo_ : name_;
}

const QString &GitHubRepoInfo::user() const
{
    return user_;
}

const QString &GitHubRepoInfo::repo() const
{
    return repo_;
}

QIcon GitHubRepoInfo::icon() const
{
    return icon_.isNull()? QIcon(":/image/github.svg") : icon_;
}
