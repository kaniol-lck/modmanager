#ifndef OPTIFINEAPI_H
#define OPTIFINEAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "optifinemodinfo.h"
#include "network/reply.hpp"

// technically, it's not an API, I just parse the html doc of optifine's website
class OptifineAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;

public:
    explicit OptifineAPI(QObject *parent = nullptr);
    static OptifineAPI *api();

    Reply<QList<OptifineModInfo> > getModList();
    Reply<QUrl> getDownloadUrl(const QString &fileName);

private:
    QNetworkAccessManager accessManager_;
};

#endif // OPTIFINEAPI_H
