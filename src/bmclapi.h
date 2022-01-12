#ifndef BMCLAPI_H
#define BMCLAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "optifine/optifinemodinfo.h"
#include "network/reply.hpp"

class BMCLAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;
public:
    explicit BMCLAPI(QObject *parent = nullptr);
    static BMCLAPI *api();

    Reply<QList<OptifineModInfo> > getOptifineList();
    Reply<QUrl> getOptifineDownloadUrl(const OptifineModInfo &info);

private:
    QNetworkAccessManager accessManager_;
};

#endif // BMCLAPI_H
