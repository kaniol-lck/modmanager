#ifndef LOCALMODPATH_H
#define LOCALMODPATH_H

#include <QObject>

#include "localmodpathinfo.h"

class LocalMod;

class LocalModPath : public QObject
{
    Q_OBJECT
public:
    explicit LocalModPath(QObject *parent = nullptr);
    explicit LocalModPath(QObject *parent, const LocalModPathInfo &info);

    void searchOnWebsites();
    void checkModUpdates();

    const LocalModPathInfo &info() const;
    void setInfo(const LocalModPathInfo &newInfo);

    const QList<LocalMod*> &modList() const;

signals:
    void modListUpdated();

    void checkWebsitesStarted();
    void websiteCheckedCountUpdated(int checkedCount);
    void websitesReady();
    void checkUpdatesStarted();
    void updateCheckedCountUpdated(int updateCount, int checkedCount);
    void updatesReady(int updateCount);

private:
    LocalModPathInfo info_;
    QList<LocalMod*> modList_;
};

#endif // LOCALMODPATH_H
