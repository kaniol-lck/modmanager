#ifndef GITHUBAPI_H
#define GITHUBAPI_H

#include <QNetworkAccessManager>
#include <QObject>

#include "githubrepoinfo.h"
#include "github/githubreleaseinfo.h"
#include "github/githubfileinfo.h"

class GitHubAPI : public QObject
{
    Q_OBJECT
public:
    explicit GitHubAPI(QObject *parent = nullptr);
    static GitHubAPI *api();

    [[nodiscard]] QMetaObject::Connection getReleases(const GitHubRepoInfo &info, int page, std::function<void (QList<GitHubReleaseInfo>)> callback);
    [[nodiscard]] QMetaObject::Connection getAssets(const QUrl assetUrl, std::function<void (QList<GitHubFileInfo>)> callback);

private:
    QNetworkAccessManager accessManager_;
};

#endif // GITHUBAPI_H
