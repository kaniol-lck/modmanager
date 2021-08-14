#include "curseforgemod.h"

#include <QNetworkRequest>
#include <QNetworkReply>

#include "util/qjsonutil.hpp"

CurseforgeMod::CurseforgeMod(QObject *parent) :
    QObject(parent),
    accessManager(new QNetworkAccessManager)
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

    QNetworkRequest request;
    QUrl url = value(value(variant, "attachments").toList().at(0), "thumbnailUrl").toUrl();

    request.setUrl(url);
    curseforgeMod->accessManager->get(request);

    return curseforgeMod;
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
    if(reply->error() != QNetworkReply::NoError) return;

    thumbnailBytes = reply->readAll();

    if(!thumbnailBytes.isEmpty())
        emit thumbnailReady();
}

const QByteArray &CurseforgeMod::getThumbnailBytes() const
{
    return thumbnailBytes;
}
