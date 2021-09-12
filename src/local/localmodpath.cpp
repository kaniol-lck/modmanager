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
            //only load loader type matched mods
            if(modInfo.loaderType() == info_.loaderType()){
                //set duplicate mod info
                if(modMap_.contains(modInfo.fabric().id())){
                    modMap_[modInfo.fabric().id()]->addDuplicateInfo(modInfo);
                    continue;
                }
                else {
                    auto mod = new LocalMod(this, modInfo);
                    connect(mod, &LocalMod::updateFinished, this, [=](bool success){
                        if(success) updatableCount_--;
                        emit updatesReady();
                    });
                    modMap_.insert(modInfo.fabric().id(), mod);
                }
                //insert fabric mod
                for(const auto &fabircModInfo : modInfo.fabricModInfoList()){
                    fabricModMap_.insert(fabircModInfo.id(), fabircModInfo);
                    provideList_ << fabircModInfo.provides();
                }
            }
        }
        //load old mods
        for(const auto &oldModInfo : qAsConst(oldInfo)){
            //TODO: other loader types
            //same loader type and same identifier
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
        duplicationCheck();
    });
}

void LocalModPath::checkFabric()
{

    //fabric
    if(info_.loaderType() == ModLoaderType::Fabric){
        //depends
        for(const auto &[fabricMod, modid, version, missingMod] : checkFabricDepends()){
            QString str;
            if(missingMod.has_value())
                str += "Missing:\n" + modid + " " + version;
            else
                str += "MisMatch:\n" + modid + " " + version;

            auto localMod = modMap_.value(fabricMod.mainId());
//            localMod->addDepend()
        }
    }
}

std::tuple<LocalModPath::FindResultType, std::optional<FabricModInfo> > LocalModPath::findFabricMod(const QString &modid, const QString &range_str) const
{
    //check contains
    if(!fabricModMap_.contains(modid) && !provideList_.contains(modid)) {
        //environment
        if(modid == "minecraft" || modid == "java" || modid == "fabricloader")
            return { Environmant, std::nullopt };
        else
            return { Missing, std::nullopt };
    }

    //current mod version
    auto modInfo = fabricModMap_.value(modid);
    auto version_str = modInfo.version();
    //remove build etc
    version_str = version_str.left(version_str.indexOf('+'));
    version_str = version_str.left(version_str.indexOf('-'));
    if(!semver::valid(version_str.toStdString())){
        return { VersionSemverError, {modInfo} };
    }
    if(!semver::valid(range_str.toStdString())){
        return { RangeSemverError, std::nullopt };
    }
    if (range_str == "*" || semver::satisfies(version_str.toStdString(), range_str.toStdString())) {
        return { Match, {modInfo} };
    } else {
        return { Mismatch, {modInfo} };
    }

}

QList<std::tuple<FabricModInfo, QString, QString, std::optional<FabricModInfo>>> LocalModPath::checkFabricDepends() const
{
    QList<std::tuple<FabricModInfo, QString, QString, std::optional<FabricModInfo>>> list;
    for(const auto &fabricMod : qAsConst(fabricModMap_)){
        //check depends
        if(fabricMod.isEmbedded()) continue;
        for(auto it = fabricMod.depends().cbegin(); it != fabricMod.depends().cend(); it++){
            auto [result, info] = findFabricMod(it.key(), it.value());
            auto modid = it.key();
            auto range_str = it.value();
            switch (result) {
            case Environmant:
                //nothing to do
                break;
            case Missing:
                list.append({ fabricMod, modid, "", std::nullopt});
                qDebug() << fabricMod.name() << fabricMod.id() << "depends" << modid << "which is missing";
                break;
            case Mismatch:
                list.append({ fabricMod, modid, range_str, info});
                qDebug() << fabricMod.name() << fabricMod.id() << "depends" << modid << "which is mismatch";
                break;
            case Match:
                //nothing to do
                break;
            case RangeSemverError:
                qDebug() << "range does not respect semver:" << modid << range_str << "provided by" << fabricMod.name();
                //nothing to do
                break;
            case VersionSemverError:
                qDebug() << "version does not respect semver:" << modid << info->version() << "provided by" << info->name();
                //nothing to do
                break;
            }
        }
    }
    return list;
}

QList<std::tuple<FabricModInfo, QString, QString, FabricModInfo> > LocalModPath::checkFabricConflicts() const
{
    QList<std::tuple<FabricModInfo, QString, QString, FabricModInfo>> list;
    for(const auto &fabricMod : qAsConst(fabricModMap_)){
        //check depends
        if(fabricMod.isEmbedded()) continue;
        for(auto it = fabricMod.conflicts().cbegin(); it != fabricMod.conflicts().cend(); it++){
            auto [result, info] = findFabricMod(it.key(), it.value());
            auto modid = it.key();
            auto range_str = it.value();
            switch (result) {
            case Environmant:
                //nothing to do
                break;
            case Missing:
                //nothing to do
            case Mismatch:
                //nothing to do
                break;
            case Match:
                list.append({ fabricMod, modid, range_str, *info});
                qDebug() << fabricMod.name() << fabricMod.id() << "conflicts" << modid << "which is present";
                break;
            case RangeSemverError:
                //nothing to do
                break;
            case VersionSemverError:
                //nothing to do
                break;
            }
        }
    }
    return list;
}

QList<std::tuple<FabricModInfo, QString, QString, FabricModInfo> > LocalModPath::checkFabricBreaks() const
{
    QList<std::tuple<FabricModInfo, QString, QString, FabricModInfo>> list;
    for(const auto &fabricMod : qAsConst(fabricModMap_)){
        //check depends
        if(fabricMod.isEmbedded()) continue;
        for(auto it = fabricMod.breaks().cbegin(); it != fabricMod.breaks().cend(); it++){
            auto [result, info] = findFabricMod(it.key(), it.value());
            auto modid = it.key();
            auto range_str = it.value();
            switch (result) {
            case Environmant:
                //nothing to do
                break;
            case Missing:
                //nothing to do
            case Mismatch:
                //nothing to do
                break;
            case Match:
                list.append({ fabricMod, modid, range_str, *info});
                qDebug() << fabricMod.name() << fabricMod.id() << "breaks" << modid << "which is present";
                break;
            case RangeSemverError:
                //nothing to do
                break;
            case VersionSemverError:
                //nothing to do
                break;
            }
        }
    }
    return list;
}

LocalMod *LocalModPath::findLocalMod(const QString &id)
{
    return modMap_.contains(id)? modMap_.value(id) : nullptr;
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
