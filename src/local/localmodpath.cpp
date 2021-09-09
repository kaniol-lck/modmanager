#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

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

void LocalModPath::updateMods(QList<QPair<LocalMod *, LocalMod::ModWebsiteType> > modUpdateList)
{
    emit updatesStarted();
    auto count = std::make_shared<int>(0);
    auto bytesReceivedList = std::make_shared<QVector<qint64>>(modUpdateList.size());

    qint64 totalSize = 0;
    for(const auto &pair : modUpdateList)
        totalSize += pair.first->updateSize(pair.second);

    for(int i = 0; i < modUpdateList.size(); i++){
        auto mod = modUpdateList.at(i).first;
        auto updateSource = modUpdateList.at(i).second;

        mod->update(updateSource);

        connect(mod, &LocalMod::updateProgress, this, [=](qint64 bytesReceived, qint64){
            (*bytesReceivedList)[i] = bytesReceived;
            auto sumReceived = std::accumulate(bytesReceivedList->cbegin(), bytesReceivedList->cend(), 0);
            emit updatesProgress(sumReceived, totalSize);
        });
        connect(mod, &LocalMod::updateFinished, this, [=]{
            (*count)++;
            emit updatesDoneCountUpdated(*count, modUpdateList.size());
            if(*count == modUpdateList.size()) emit updatesDone(*count);
        });
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
        //only load available mod files
        for(const auto &fileInfo : QDir(info_.path()).entryInfoList({ "*.jar" }, QDir::Files))
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

        Config config;
        auto autoSearchOnWebsites = config.getAutoSearchOnWebsites();
        auto autoCheckUpdate = config.getAutoCheckUpdate();

        if(autoSearchOnWebsites)
            searchOnWebsites();

        if(autoCheckUpdate){
            connect(this, &LocalModPath::websitesReady, [=]{
                checkModUpdates();
                disconnect(SIGNAL(websitesReady()));
            });
        }

    });
}

const QList<LocalMod*> &LocalModPath::modList() const
{
    return modList_;
}
