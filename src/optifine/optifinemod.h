#ifndef OPTIFINEMOD_H
#define OPTIFINEMOD_H

#include <QObject>

#include "network/reply.hpp"
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

    std::unique_ptr<Reply<QUrl>> downloadUrlGetter_;
};

#endif // OPTIFINEMOD_H
