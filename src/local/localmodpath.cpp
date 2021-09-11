#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "cpp-semver.hpp"
#include "curseforge/curseforgeapi.h"
#include "modrinth/modrinthapi.h"
#include "config.h"

LocalModPath::LocalModPath(QObject *parent, const LocalModPathInfo &info) :
    QObject(parent),
    curseforgeAPI_(new CurseforgeAPI(this)),
    modrinthAPI_(new ModrinthAPI(this)),
    info_(info)
{
    loadMods();
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
        fabricModMap_.clear();
        provideList_.clear();
        auto [info, oldInfo] = watcher->result();
        //load mods
        for(const auto &modInfo : qAsConst(info)){
            //TODO: other loader types
            if(modInfo.loaderType() == info_.loaderType()){
                auto mod = new LocalMod(this, modInfo);
                connect(mod, &LocalMod::updateFinished, this, [=](bool success){
                    if(success) updatableCount_--;
                    emit updatesReady();
                });

                modMap_.insert(modInfo.fabric().id(), mod);
                for(const auto &fabircModInfo : modInfo.fabricModInfoList()){
                    fabricModMap_.insert(fabircModInfo.id(), fabircModInfo);
                    provideList_ << fabircModInfo.provides();
                }
            }
        }
        //load old mods
        for(const auto &oldModInfo : qAsConst(oldInfo)){
            //TODO: other loader types
            if(oldModInfo.loaderType() == info_.loaderType() && modMap_.contains(oldModInfo.fabric().id()))
                modMap_.value(oldModInfo.fabric().id())->addOldInfo(oldModInfo);
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

        //fabric
        for(auto fabricMod : fabricModMap_){
            //check depends
            if(fabricMod.isEmbedded()) continue;
            for(auto it = fabricMod.depends().cbegin(); it != fabricMod.depends().cend(); it++){
                auto modid = it.key();
                if(!fabricModMap_.contains(modid) && !provideList_.contains(modid)) {
                    if(modid == "minecraft"){
                        //TODO
                    } else if(modid == "java"){
                        //TODO
                    } else if(modid == "fabricloader"){
                        //TODO
//                    } else if(modid.startsWith("fabric-")){
                        //TODO :fabric API
                        //embedded jar
                    } else{
                        qDebug() << fabricMod.name() << fabricMod.id() << "depends" << modid;
                    }
                    continue;
                }
                //current mod version
                auto version_str = fabricModMap_.value(modid).version();
                //remove build etc
                version_str.remove(QRegExp(R"(\+.*$)")).remove(R"(\-.*$)");
                if(!semver::valid(version_str.toStdString())){
                    qDebug() << fabricMod.name() << "does not respect semver:" << version_str;
                    continue;
                }
                auto range_str = it.value();
                if(!semver::valid(range_str.toStdString())){
                    qDebug() << fabricMod.name() << "does not respect semver:" << range_str;
                    continue;
                }
                if (range_str == "*" || semver::satisfies(version_str.toStdString(), range_str.toStdString())) {
                    //pass
                } else {
                    qDebug() << fabricMod.name() << "failed" << modid;
                    qDebug() << "current:" << version_str;
                    qDebug() << "depends:" << it.value();
                }


            }
            //check conflicts
            for(auto it = fabricMod.conflicts().cbegin(); it != fabricMod.conflicts().cend(); it++){

            }
            //check breaks
            for(auto it = fabricMod.breaks().cbegin(); it != fabricMod.breaks().cend(); it++){

            }
        }

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
    if(info_ == newInfo) return;

    info_ = newInfo;
    emit infoUpdated();

    //path, game version or loader type change will trigger mod reload
    if(info_.path() != newInfo.path() || info_.gameVersion() != newInfo.gameVersion() || info_.loaderType() != newInfo.loaderType())
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

QMap<QString, QList<LocalMod*>> LocalModPath::duplicationCheck() const
{
    QMap<QString, QList<LocalMod*>> map;
    for(const auto &id : modMap_.uniqueKeys()){
        if(modMap_.values(id).size() > 1)
            map[id] = modMap_.values(id);
    }
    return map;
}

void LocalModPath::deleteAllOld() const
{
    for(auto mod : modMap_)
        mod->deleteAllOld();
    //TODO: inform finished
}
