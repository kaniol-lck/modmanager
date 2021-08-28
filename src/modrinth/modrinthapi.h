#ifndef MODRINTHAPI_H
#define MODRINTHAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "modrinthmodinfo.h"
#include "gameversion.h"
#include "modloadertype.h"

class ModrinthAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;
public:
    static ModrinthAPI *api();

    static void searchMods(const QString name, int index, int sort, std::function<void (QList<ModrinthModInfo>)> callback);

private:
    explicit ModrinthAPI(QObject *parent = nullptr);
    QNetworkAccessManager accessManager;
};

#endif // MODRINTHAPI_H
