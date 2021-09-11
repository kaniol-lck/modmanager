#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "curseforge/curseforgeapi.h"
#include "modrinth/modrinthapi.h"
#include "config.h"

LocalModPath::LocalModPath(QObject *parent, const LocalModPathInfo &info) :
    QObject(parent),
    curseforgeAPI_(new CurseforgeAPI(this)),
    modrinthAPI_(new ModrinthAPI(this))
{
    setInfo(info);
}

void LocalModPath::loadMods()
{
    auto future = QtConcurrent::run([&]{
        QList<LocalModInfo> infoList;
        //only load available mod files
        for(const auto &fileInfo : QDir(info_.path()).entryInfoList({ "*.jar" }, QDir::Files))
            infoList << LocalModInfo(fileInfo.absoluteFilePath());

        QList<LocalModInfo> oldInfoList;
        //only load old mod files
        for(const auto &fileInfo : QDir(info_.path()).entryInfoList({ "*.jar.old" }, QDir::Files))
            oldInfoList << LocalModInfo(fileInfo.absoluteFilePath());
        return std::tuple{ infoList, oldInfoList };
    });
    auto watcher = new QFutureWatcher<std::tuple<QList<LocalModInfo>, QList<LocalModInfo>>>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<std::tuple<QList<LocalModInfo>, QList<LocalModInfo>>>::finished, this, [=]{
        modMap_.clear();
        auto [info, oldInfo] = watcher->result();
        //load mods
        for(const auto &modInfo : qAsConst(info)){
            //TODO: other loader types
            if(modInfo.isFabricMod()){
                auto mod = new LocalMod(this, modInfo);
                connect(mod, &LocalMod::updateFinished, this, [=](bool success){
                    if(success) updatableCount_--;
                    emit updatesReady();
                });

                modMap_.insert(modInfo.id(), mod);
            }
        }
        //load old mods
        for(const auto &oldModInfo : qAsConst(oldInfo)){
            //TODO: other loader types
            if(oldModInfo.isFabricMod() && modMap_.contains(oldModInfo.id()))
                modMap_.value(oldModInfo.id())->addOldInfo(oldModInfo);
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

        duplicationCheck();

    });
}

void LocalModPath::searchOnWebsites()
{
    emit checkWebsitesStarted();
    auto count = std::make_shared<int>(0);
    for(const auto &mod : qAsConst(modMap_)){
        connect(mod, &LocalMod::websiteReady, this, [=]{
            (*count)++;
            emit websiteCheckedCountUpdated(*count);
            if(*count == modMap_.size()) emit websitesReady();
        });
        mod->searchOnWebsite();
    }
}

void LocalModPath::checkModUpdates()
{
    emit checkUpdatesStarted();
    auto count = std::make_shared<int>(0);
    auto updateCount = std::make_shared<int>(0);
    for(const auto &mod : qAsConst(modMap_)){
        connect(mod, &LocalMod::updateReady, this, [=](bool bl){
            (*count)++;
            if(bl) (*updateCount)++;
            emit updateCheckedCountUpdated(*updateCount, *count);
            //done
            if(*count == modMap_.size()){
                updatableCount_ = *updateCount;
                emit updatesReady();
            }
        });
        mod->checkUpdates(info_.gameVersion(), info_.loaderType());
    }
}

void LocalModPath::updateMods(QList<QPair<LocalMod *, LocalMod::ModWebsiteType> > modUpdateList)
{
    emit updatesStarted();
    auto count = std::make_shared<int>(0);
    auto successCount = std::make_shared<int>(0);
    auto failCount = std::make_shared<int>(0);
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
        connect(mod, &LocalMod::updateFinished, this, [=](bool success){
            (*count)++;
            if(success)
                (*successCount)++;
            else
                (*failCount)++;
            emit updatesDoneCountUpdated(*count, modUpdateList.size());
            if(*count == modUpdateList.size())
                emit updatesDone(*successCount, *failCount);
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
    loadMods();
}

CurseforgeAPI *LocalModPath::curseforgeAPI() const
{
    return curseforgeAPI_;
}

ModrinthAPI *LocalModPath::modrinthAPI() const
{
    return modrinthAPI_;
}

int LocalModPath::updatableCount() const
{
    return updatableCount_;
}

const QMap<QString, LocalMod *> &LocalModPath::modMap() const
{
    return modMap_;
}

void LocalModPath::duplicationCheck() const
{
    for(const auto &id : modMap_.uniqueKeys()){
        if(modMap_.values(id).size() > 1)
            qDebug() << "duplicate: " << id;
    }
}

void LocalModPath::deleteAllOld() const
{
    for(auto mod : modMap_)
        mod->deleteAllOld();
    //TODO: inform finished
}
