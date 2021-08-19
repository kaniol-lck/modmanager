#ifndef CURSEFORGEMOD_H
#define CURSEFORGEMOD_H

#include <QObject>
#include <QVariant>
#include <QUrl>

#include "curseforgemodinfo.h"

class QNetworkAccessManager;

class CurseforgeMod : public QObject
{
    Q_OBJECT
public:
    CurseforgeMod(QObject *parent, QNetworkAccessManager *manager, const CurseforgeModInfo &info);

    void downloadThumbnail();
    void getDescription();

    const CurseforgeModInfo &getModInfo() const;

signals:
    void thumbnailReady();
    void descriptionReady();

private:
    QNetworkAccessManager *accessManager;

    CurseforgeModInfo curseforgeModInfo;

};

#endif // CURSEFORGEMOD_H
