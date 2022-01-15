#ifndef CURSEFORGEFILE_H
#define CURSEFORGEFILE_H

#include <QObject>

#include "curseforgefileinfo.h"
#include "network/reply.hpp"

class CurseforgeAPI;
class CurseforgeFile : public QObject
{
    Q_OBJECT
public:
    explicit CurseforgeFile(QObject *parent, int projectID, int fileID);

    std::shared_ptr<Reply<CurseforgeFileInfo>> acquireFileInfo();

    const CurseforgeFileInfo &info() const;

signals:
    void infoReady();

private:
    CurseforgeAPI *api_;
    int projectID_;
    CurseforgeFileInfo info_;
    std::shared_ptr<Reply<CurseforgeFileInfo> > infoGetter_;
};

#endif // CURSEFORGEFILE_H
