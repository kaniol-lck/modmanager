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

void LocalModPath::reload()
{
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
        modList_.clear();
        for(const auto &modInfo : watcher->result()){
            //TODO: other loader types
            if(modInfo.isFabricMod()){
                auto mod = new LocalMod(this, modInfo);
                connect(mod, &LocalMod::updateFinished, this, [=](bool success){
                    if(success) updatableCount_--;
                    emit updatesReady();
                });

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
            //done
            if(*count == modList_.size()){
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
    reload();
}

const QList<LocalMod*> &LocalModPath::modList() const
{
    return modList_;
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
