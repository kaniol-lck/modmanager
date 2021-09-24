#include "replaymod.h"

#include "replay/replayapi.h"

ReplayMod::ReplayMod(QObject *parent) :
    QObject(parent)
{}

ReplayMod::ReplayMod(QObject *parent, const ReplayModInfo &info) :
    QObject(parent),
    api_(ReplayAPI::api()),
    modInfo_(info)
{}

const ReplayModInfo &ReplayMod::modInfo() const
{
    return modInfo_;
}
