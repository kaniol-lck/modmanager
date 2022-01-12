#ifndef GITHUBAPI_H
#define GITHUBAPI_H

#include <QNetworkAccessManager>
#include <QObject>

#include "githubrepoinfo.h"
#include "github/githubreleaseinfo.h"
#include "github/githubfileinfo.h"
#include "network/reply.hpp"

class GitHubAPI : public QObject
{
    Q_OBJECT
public:
    explicit GitHubAPI(QObject *parent = nullptr);
    static GitHubAPI *api();

    Reply<QList<GitHubReleaseInfo> > getReleases(const GitHubRepoInfo &info, int page);
    Reply<QList<GitHubFileInfo> > getAssets(const QUrl assetUrl);

private:
    QNetworkAccessManager accessManager_;
};

#endif // GITHUBAPI_H
