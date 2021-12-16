#ifndef GITHUBRELEASE_H
#define GITHUBRELEASE_H

#include <QObject>

#include "githubreleaseinfo.h"

class GitHubRelease : public QObject
{
    Q_OBJECT
public:
    explicit GitHubRelease(QObject *parent, const GitHubReleaseInfo &info);

    const GitHubReleaseInfo &info() const;

signals:
private:
    GitHubReleaseInfo info_;
};

#endif // GITHUBRELEASE_H
