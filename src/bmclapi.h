#ifndef BMCLAPI_H
#define BMCLAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "optifine/optifinemodinfo.h"

class BMCLAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;
public:
    explicit BMCLAPI(QObject *parent = nullptr);
    static BMCLAPI *api();

    void getOptifineList(std::function<void (QList<OptifineModInfo>)> callback);
    void getOptifineDownloadUrl(const OptifineModInfo &info, std::function<void (QUrl)> callback);

private:
    QNetworkAccessManager accessManager_;
};

#endif // BMCLAPI_H
