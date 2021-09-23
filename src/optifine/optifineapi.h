#ifndef OPTIFINEAPI_H
#define OPTIFINEAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "optifinemodinfo.h"

// technically, it's not an API, I just parse the html doc of optifine's website
// TODO: BMCLAPI
class OptifineAPI : public QObject
{
    Q_OBJECT
public:
    explicit OptifineAPI(QObject *parent = nullptr);
    static OptifineAPI *api();

    void getModList(std::function<void (QList<OptifineModInfo>)> callback);
    void getDownloadUrl(const QString &fileName, std::function<void (QUrl)> callback);

signals:

private:
    QNetworkAccessManager accessManager_;

};

#endif // OPTIFINEAPI_H
