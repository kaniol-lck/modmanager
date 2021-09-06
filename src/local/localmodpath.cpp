#include "localmodpath.h"

#include <QDir>

#include "localmod.h"

LocalModPath::LocalModPath(QObject *parent) : QObject(parent)
{}

LocalModPath::LocalModPath(QObject *parent, const LocalModPathInfo &info) :
    QObject(parent)
{
    setInfo(info);
}

const LocalModPathInfo &LocalModPath::info() const
{
    return info_;
}

void LocalModPath::setInfo(const LocalModPathInfo &newInfo)
{
    info_ = newInfo;

    for(const auto &fileInfo : QDir(info_.path()).entryInfoList(QDir::Files)){
        auto modInfo = LocalModInfo(fileInfo.absoluteFilePath());
        //TODO: other loader types
        if(modInfo.isFabricMod()){
            auto mod = new LocalMod(this, modInfo);
            modList_ << mod;
        }
    }
}

const QList<LocalMod*> &LocalModPath::modList() const
{
    return modList_;
}
