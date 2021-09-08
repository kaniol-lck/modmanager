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
    auto count = std::make_shared<int>(modList_.size());
    for(const auto &mod : qAsConst(modList_)){
        connect(mod, &LocalMod::websiteReady, this, [=]{
            if(--(*count) == 0) emit websitesReady();
        });
        mod->searchOnWebsite();
    }
}

void LocalModPath::checkModUpdates(const GameVersion &targetVersion, ModLoaderType::Type targetType)
{
    emit checkUpdatesStarted();
    auto count = std::make_shared<int>(modList_.size());
    for(const auto &mod : qAsConst(modList_)){
        connect(mod, &LocalMod::updateReady, this, [=]{
            if(--(*count) == 0) emit updatesReady();
        });
        mod->checkUpdates(targetVersion, targetType);
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
                checkModUpdates(info_.gameVersion(), info_.loaderType());
        });

        searchOnWebsites();
    });
}

const QList<LocalMod*> &LocalModPath::modList() const
{
    return modList_;
}
