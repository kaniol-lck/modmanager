#ifndef GITHUBRELEASEINFO_H
#define GITHUBRELEASEINFO_H

#include "github/githubfileinfo.h"

class GitHubReleaseInfo
{
public:
    GitHubReleaseInfo() = default;

    static GitHubReleaseInfo fromVariant(const QVariant &variant);

    const QString &name() const;
    const QString &tagName() const;
    const QString &body() const;
    const QUrl &url() const;
    bool prerelease() const;
    const QList<GitHubFileInfo> &assets() const;

private:
    QString name_;
    QString tagName_;
    QString body_;
    QUrl url_;
    QList<GitHubFileInfo> assets_;
    bool prerelease_;
};

#endif // GITHUBRELEASEINFO_H
