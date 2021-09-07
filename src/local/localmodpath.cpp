#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

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
    modList_.clear();
    auto future = QtConcurrent::run([&]{
        QList<LocalModInfo> infoList;
        for(const auto &fileInfo : QDir(info_.path()).entryInfoList(QDir::Files))
            infoList << LocalModInfo(fileInfo.absoluteFilePath());
        return infoList;
    });
    auto watcher = new QFutureWatcher<QList<LocalModInfo>>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<QList<LocalModInfo>>::finished, this, [=]{
        for(const auto &modInfo : watcher->result()){
            //TODO: other loader types
            if(modInfo.isFabricMod()){
                auto mod = new LocalMod(this, modInfo);
                modList_ << mod;
            }
        }
        emit modListUpdated();
    });
}

const QList<LocalMod*> &LocalModPath::modList() const
{
    return modList_;
}
