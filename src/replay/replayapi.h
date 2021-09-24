#ifndef REPLAYAPI_H
#define REPLAYAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <functional>

#include "replaymodinfo.h"

// technically, it's not an API, I just parse the html doc of replay's website
class ReplayAPI : public QObject
{
    Q_OBJECT
    static const QString PREFIX;

public:
    explicit ReplayAPI(QObject *parent = nullptr);
    static ReplayAPI *api();

    void getModList(std::function<void (QList<ReplayModInfo>)> callback);

private:
    QNetworkAccessManager accessManager_;
};

#endif // REPLAYAPI_H
