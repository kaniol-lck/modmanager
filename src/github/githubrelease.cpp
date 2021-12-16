#include "githubrelease.h"

GitHubRelease::GitHubRelease(QObject *parent, const GitHubReleaseInfo &info) :
    QObject(parent),
    info_(info)
{}

const GitHubReleaseInfo &GitHubRelease::info() const
{
    return info_;
}
