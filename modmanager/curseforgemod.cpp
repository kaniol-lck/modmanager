#include "curseforgemod.h"

#include <QNetworkRequest>
#include <QNetworkReply>

#include "util/qjsonutil.hpp"
#include "util/downloader.h"

CurseforgeMod::CurseforgeMod(QObject *parent) :
    QObject(parent),
    accessManager(new QNetworkAccessManager(this))
{
    connect(accessManager, &QNetworkAccessManager::finished, this, &CurseforgeMod::thumbnailDownloadFinished);
}

CurseforgeMod *CurseforgeMod::fromFingerprint(QObject *parent, QString murmurhash)
{
    auto curseforgeMod = new CurseforgeMod(parent);

    //wip

    return curseforgeMod;
}

CurseforgeMod *CurseforgeMod::fromVariant(QObject *parent, QVariant variant)
{
    auto curseforgeMod = new CurseforgeMod(parent);

    curseforgeMod->id = value(variant, "id").toInt();
    curseforgeMod->name = value(variant, "name").toString();
    curseforgeMod->summary = value(variant, "summary").toString();
    curseforgeMod->websiteUrl = value(variant, "websiteUrl").toUrl();

    //authors
    auto authorsList = value(variant, "authors").toList();
    for(const auto &author : qAsConst(authorsList))
        curseforgeMod->authors << value(author, "name").toString();

    //thumbnail image
    auto attachmentsList = value(variant, "attachments").toList();
    if(!attachmentsList.isEmpty())
        curseforgeMod->thumbnailUrl = value(attachmentsList.at(0), "thumbnailUrl").toUrl();

    //latest file url
    auto latestFileslist = value(variant, "latestFiles").toList();
    if (!latestFileslist.isEmpty()){
        curseforgeMod->latestFileUrl = value(latestFileslist.at(0), "downloadUrl").toUrl();
        curseforgeMod->latestFileName = value(latestFileslist.at(0), "fileName").toString();
        curseforgeMod->latestFileLength = value(latestFileslist.at(0), "fileLength").toInt();
    }

    return curseforgeMod;
}

void CurseforgeMod::downloadThumbnail()
{
    if(thumbnailUrl.isEmpty()) return;

    QNetworkRequest request(thumbnailUrl);
    accessManager->get(request);
}

int CurseforgeMod::getId() const
{
    return id;
}

const QString &CurseforgeMod::getName() const
{
    return name;
}

const QString &CurseforgeMod::getSummary() const
{
    return summary;
}

void CurseforgeMod::thumbnailDownloadFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError) {
        return;
    }

    thumbnailBytes = reply->readAll();

    if(!thumbnailBytes.isEmpty())
        emit thumbnailReady();
}

int CurseforgeMod::getLatestFileLength() const
{
    return latestFileLength;
}

const QString &CurseforgeMod::getLatestFileName() const
{
    return latestFileName;
}

const QUrl &CurseforgeMod::getLatestFileUrl() const
{
    return latestFileUrl;
}

const QString &CurseforgeMod::getDescription() const
{
    return description;
}

void CurseforgeMod::setDescription(const QString &newDescription)
{
    description = newDescription;
}

const QStringList &CurseforgeMod::getAuthors() const
{
    return authors;
}

const QUrl &CurseforgeMod::getWebsiteUrl() const
{
    return websiteUrl;
}

const QByteArray &CurseforgeMod::getThumbnailBytes() const
{
    return thumbnailBytes;
}
