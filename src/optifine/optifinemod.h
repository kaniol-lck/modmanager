#ifndef OPTIFINEMOD_H
#define OPTIFINEMOD_H

#include <QObject>

#include "optifinemodinfo.h"

class OptifineAPI;
class BMCLAPI;

class OptifineMod : public QObject
{
    Q_OBJECT
public:
    explicit OptifineMod(QObject *parent = nullptr);
    explicit OptifineMod(QObject *parent, const OptifineModInfo &info);

    void acquireDownloadUrl();

    const OptifineModInfo &modInfo() const;

signals:
    void downloadUrlReady();

private:
    OptifineAPI *api_;
    BMCLAPI *bmclapi_;
    OptifineModInfo modInfo_;

    bool gettingDownloadUrl_ = false;
};

#endif // OPTIFINEMOD_H
