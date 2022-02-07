#include "githubapi.h"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

#include "config.hpp"

GitHubAPI::GitHubAPI(QObject *parent) : QObject(parent)
{
    accessManager_.setTransferTimeout(Config().getNetworkRequestTimeout());
}

GitHubAPI *GitHubAPI::api()
{
    static GitHubAPI api;
    return &api;
}

Reply<QList<GitHubReleaseInfo> > GitHubAPI::getReleases(const GitHubRepoInfo &info, int page)
{
    QUrl releaseUrl = QString("https://api.github.com/repos/%1/%2/releases").arg(info.user(), info.repo());

    //url query
    QUrlQuery urlQuery;

    //index
    urlQuery.addQueryItem("page", QString::number(page));
    //search page size
    urlQuery.addQueryItem("per_page", QString::number(Config().getSearchResultCount()));

    releaseUrl.setQuery(urlQuery);
    QNetworkRequest request(releaseUrl);
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<GitHubReleaseInfo>{};
        }
        auto result = jsonDocument.toVariant();

        QList<GitHubReleaseInfo> list;
        for(auto &&variant : result.toList())
            list << GitHubReleaseInfo::fromVariant(variant);
        return list;
    } };
}

Reply<QList<GitHubFileInfo> > GitHubAPI::getAssets(const QUrl assetUrl)
{
    QNetworkRequest request(assetUrl);
    auto reply = accessManager_.get(request);
    return { reply, [=]{
        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return QList<GitHubFileInfo>{};
        }
        QList<GitHubFileInfo> list;
        for(auto &&variant : jsonDocument.toVariant().toList())
            list << GitHubFileInfo::fromVariant(variant);
        return list;
    } };
}
