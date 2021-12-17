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

QMetaObject::Connection GitHubAPI::getReleases(const GitHubRepoInfo &info, int page, std::function<void (QList<GitHubReleaseInfo>)> callback)
{
    QUrl releaseUrl = QString("https://api.github.com/repos/%1/%2/releases").arg(info.user(), info.repo());

    //url query
    QUrlQuery urlQuery;

    //index
    urlQuery.addQueryItem("page", QString::number(page));
    //search page size
    urlQuery.addQueryItem("per_page", QString::number(Config().getSearchResultCount()));

    releaseUrl.setQuery(urlQuery);
    qDebug() << releaseUrl;
    QNetworkRequest request(releaseUrl);
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }
        auto result = jsonDocument.toVariant();

        QList<GitHubReleaseInfo> list;
        for(auto &&variant : result.toList())
            list << GitHubReleaseInfo::fromVariant(variant);
        callback(list);
    });
}

QMetaObject::Connection GitHubAPI::getAssets(const QUrl assetUrl, std::function<void (QList<GitHubFileInfo>)> callback)
{
    QNetworkRequest request(assetUrl);
    auto reply = accessManager_.get(request);
    return connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            return;
        }

        //parse json
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug("%s", error.errorString().toUtf8().constData());
            return;
        }
        auto result = jsonDocument.toVariant();

        QList<GitHubFileInfo> list;
        for(auto &&variant : result.toList())
            list << GitHubFileInfo::fromVariant(variant);
        callback(list);
    });
}
