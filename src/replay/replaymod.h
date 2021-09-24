#ifndef REPLAYMOD_H
#define REPLAYMOD_H

#include <QObject>

#include "replaymodinfo.h"

class ReplayAPI;

class ReplayMod : public QObject
{
    Q_OBJECT
public:
    explicit ReplayMod(QObject *parent = nullptr);
    explicit ReplayMod(QObject *parent, const ReplayModInfo &info);

    const ReplayModInfo &modInfo() const;

signals:

private:
    ReplayAPI *api_;
    ReplayModInfo modInfo_;

};

#endif // REPLAYMOD_H
