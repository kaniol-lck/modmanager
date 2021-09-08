#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "localmod.h"
#include "config.h"

LocalModPath::LocalModPath(QObject *parent) : QObject(parent)
{}

LocalModPath::LocalModPath(QObject *parent, const LocalModPathInfo &info) :
    QObject(parent)
{
    setInfo(info);
}

void LocalModPath::searchOnWebsites()
{
    emit checkWebsitesStarted();
    auto count = std::make_shared<int>(0);
    for(const auto &mod : qAsConst(modList_)){
        connect(mod, &LocalMod::websiteReady, this, [=]{
            (*count)++;
            emit websiteCheckedCountUpdated(*count);
            if(*count == modList_.size()) emit websitesReady();
        });
        mod->searchOnWebsite();
    }
}

void LocalModPath::checkModUpdates()
{
    emit checkUpdatesStarted();
    auto count = std::make_shared<int>(0);
    auto updateCount = std::make_shared<int>(0);
    for(const auto &mod : qAsConst(modList_)){
        connect(mod, &LocalMod::updateReady, this, [=](bool bl){
            (*count)++;
            if(bl) (*updateCount)++;
            emit updateCheckedCountUpdated(*updateCount, *count);
            if(*count == modList_.size()) emit updatesReady(*updateCount);
        });
        mod->checkUpdates(info_.gameVersion(), info_.loaderType());
    }
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

        auto autoCheckUpdate = Config().getAutoCheckUpdate();
        connect(this, &LocalModPath::websitesReady, this, [=]{
            if(autoCheckUpdate)
                checkModUpdates();
        });

        searchOnWebsites();
    });
}

const QList<LocalMod*> &LocalModPath::modList() const
{
    return modList_;
}
