#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "localmodpathmanager.h"
#include "cpp-semver.hpp"
#include "localmodfile.h"
#include "curseforge/curseforgeapi.h"
#include "modrinth/modrinthapi.h"
#include "util/tutil.hpp"
#include "util/funcutil.h"
#include "util/checksheet.h"
#include "config.hpp"

LocalModPath::LocalModPath(const LocalModPathInfo &info) :
    QObject(LocalModPathManager::manager()),
    curseforgeAPI_(new CurseforgeAPI(this)),
    modrinthAPI_(new ModrinthAPI(this)),
    info_(info)
{
    watcher_.addPath(info_.path());
    connect(&watcher_, &QFileSystemWatcher::directoryChanged, this, &LocalModPath::onDirectoryChanged);
    connect(this, &LocalModPath::loadFinished, [=]{
        auto conn = connect(&modsLinker_, &CheckSheet::finished, this, [=]{
            auto interval = Config().getUpdateCheckInterval();
            if(interval == Config::Always || !latestUpdateCheck_.isValid() ||
                    (interval == Config::EveryDay && latestUpdateCheck_.daysTo(QDateTime::currentDateTime()) >= 1))
                checkModUpdates();
            else if(interval != Config::Never){
            }
        });
        qDebug() << "load finished";
        linkAllFiles();
        connect(&modsLinker_, &CheckSheet::finished, this, disconnecter(conn));
        connect(&updateChecker_, &CheckSheet::finished, this, [=]{
            latestUpdateCheck_ = QDateTime::currentDateTime();
            writeToFile();
        });
//        connect(&modsLinker_, &CheckSheet::finished, this, &LocalModPath::updatesReady);
    });
}

LocalModPath::LocalModPath(LocalModPath *path, const QString &subDir) :
    QObject(path),
    relative_(path->relative_ + QStringList{subDir}),
    curseforgeAPI_(path->curseforgeAPI_),
    modrinthAPI_(path->modrinthAPI_),
    info_(path->info_)
{
    connect(&watcher_, &QFileSystemWatcher::directoryChanged, this, &LocalModPath::onDirectoryChanged);
    addSubTagable(path);
    importTag(Tag(subDir, TagCategory::SubDirCategory));
    info_.path_.append("/").append(relative_.join("/"));
    watcher_.addPath(info_.path());
}

const QStringList &LocalModPath::relative() const
{
    return relative_;
}

const QMap<QString, LocalModPath *> &LocalModPath::subPaths() const
{
    return subPaths_;
}

const CheckSheet *LocalModPath::modsLinker() const
{
    return &modsLinker_;
}

QString LocalModPath::displayName() const
{
    return info_.displayName();
}

QIcon LocalModPath::icon() const
{
    return info_.icon();
}

const CheckSheet *LocalModPath::updateChecker() const
{
    return &updateChecker_;
}

bool LocalModPath::isLinking() const
{
    return modsLinker_.isWaiting();
}

bool LocalModPath::isUpdating() const
{
    return isUpdating_;
}

bool LocalModPath::modsLoaded() const
{
    return loaded_;
}

LocalModPath::~LocalModPath()
{
    qDeleteAll(modMap_);
}

QString LocalModPath::modsJsonFilePath() const
{
    QDir dir(info_.path());
    return dir.absoluteFilePath(kFileName);
}

void LocalModPath::loadMods(bool autoLoaderType)
{
    if(isLoading_) return;
    qDebug() << "load" << info_.displayName();
    loaded_ = true;
    isLoading_ = true;
    modsLinker_.reset();
    updateChecker_.reset();
    isUpdating_ = false;
    QDir dir(info_.path());
    for(auto &&fileInfo : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)){
        bool containsMod = false;
        for(auto &&fileInfo2 : fileInfo.dir().entryInfoList(QDir::Files))
            if((containsMod = LocalModFile::availableSuffix.contains(fileInfo2.suffix())))
                break;
        if(auto fileName = fileInfo.fileName(); !subPaths_.contains(fileName) && containsMod){
            auto subPath = new LocalModPath(this, fileName);
            subPaths_.insert(fileName, subPath);
            containedTags_.addSubTagable(subPath);
            containedTags_.addSubTagable(&subPath->containedTags_);
            subPath->loadMods();
        }
    }
    QList<LocalModFile*> modFileList;
    for(auto &&fileInfo : dir.entryInfoList(QDir::Files))
        if(LocalModFile::availableSuffix.contains(fileInfo.suffix()))
            modFileList << new LocalModFile(this, fileInfo.absoluteFilePath(), relative_);

    auto future = QtConcurrent::run([=]{
        int count = 0;
        QVector<int> modCount(3, 0);
        emit loadStarted();
        for(auto &file : modFileList){
            modCount[file->loadInfo()]++;
            emit loadProgress(++count, modFileList.size());
        }
        if(autoLoaderType){
            if(auto iter = std::max_element(modCount.cbegin(), modCount.cend()); iter != modCount.cend()){
                auto loaderType = ModLoaderType::local.at(iter - modCount.cbegin());
                info_.setLoaderType(loaderType);
                emit infoUpdated();
            }
        }
        for(const auto &file : qAsConst(modFileList))
            file->setLoaderType(info_.loaderType());
    });

    auto watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<void>::finished, this, [=]{
        qDebug() << "modMap clear";
        for(const auto &mod : qAsConst(modMap_))
            containedTags_.removeSubTagable(mod);
        qDeleteAll(modMap_);
        modMap_.clear();
        if(optiFineMod_){
            containedTags_.removeSubTagable(optiFineMod_);
            optiFineMod_->deleteLater();
            optiFineMod_ = nullptr;
        }
        fabricModMap_.clear();
        provideList_.clear();

        //link all file from cache
        for(const auto &file : qAsConst(modFileList))
            file->linker()->linkCached();

        //load normal first
        //load normal mods
        for(const auto &file : qAsConst(modFileList))
            addModFile(file);

        //delete unused files
        for(const auto &file : qAsConst(modFileList)){
            if(!file->parent())
                file->deleteLater();
        }

        //restore cached info
        readFromFile();
        updateUpdatableCount();
        emit modListUpdated();
        isLoading_ = false;
        emit loadFinished();
    });
}

void LocalModPath::addModFile(LocalModFile *file)
{
    //not mod
    if(auto type = file->type(); type == LocalModFile::Downloading || type == LocalModFile::NotMod) return;
    //no fabric / forge info
    if(file->loaderType() == ModLoaderType::Any) return;
    //load optifine seperately under fabric
    if(info_.loaderType() == ModLoaderType::Fabric && file->commonInfo()->id() == "optifine"){
        if(optiFineMod_)
            optiFineMod_->addDuplicateFile(file);
        else{
            optiFineMod_ = new LocalMod(this);
            optiFineMod_->addModFile(file);
            containedTags_.addSubTagable(optiFineMod_);
            //connect update signal
            connect(optiFineMod_, &LocalMod::updateFinished, this, [=](bool){
                emit updatesReady();
            });
            connect(optiFineMod_, &LocalMod::updateReady, this, &LocalModPath::updateUpdatableCount);
            connect(optiFineMod_, &LocalMod::updateFinished, this, &LocalModPath::updateUpdatableCount);
        }
        return;
    }
    if(file->loaderType() != info_.loaderType() && info_.loaderType() != ModLoaderType::Any) return;
    auto id = file->commonInfo()->id();
    //mod in path
    if(modMap_.contains(id)){
        modMap_[id]->addModFile(file);
        return;
    }else {
        //new mod
        auto mod = new LocalMod(this);
        mod->addModFile(file);
        //connect update signal
        connect(mod, &LocalMod::updateFinished, this, [=](bool){
            emit updatesReady();
        });
        connect(mod, &LocalMod::updateReady, this, &LocalModPath::updateUpdatableCount);
        connect(mod, &LocalMod::updateFinished, this, &LocalModPath::updateUpdatableCount);
        modMap_[id] = mod;
        containedTags_.addSubTagable(mod);
    }
}

void LocalModPath::removeModFile(LocalModFile *file)
{
    for(const auto &mod : qAsConst(modMap_))
        if(mod->files().contains(file)){
            mod->removeModFile(file);
            if(!mod->modFile())
                modMap_.remove(file->commonInfo()->id());
            return;
        }
    if(optiFineMod_ && optiFineMod_->files().contains(file)){
        optiFineMod_->removeModFile(file);
        if(!optiFineMod_->modFile())
            optiFineMod_ = nullptr;
    }
}

LocalModPath *LocalModPath::addSubPath(const QString &relative)
{
    auto subPath = new LocalModPath(this, relative);
    QDir().mkpath(subPath->info().path());
    subPaths_.insert(relative, subPath);
    containedTags_.addSubTagable(subPath);
    containedTags_.addSubTagable(&subPath->containedTags_);
    subPath->loadMods();
    return subPath;
}

void LocalModPath::checkFabric()
{
    //fabric
    if(info_.loaderType() == ModLoaderType::Fabric){
        //depends
        for(auto &&[fabricMod, modid, version, missingMod] : checkFabricDepends()){
            QString str;
            if(missingMod.has_value())
                str += "Missing:\n" + modid + " " + version;
            else
                str += "MisMatch:\n" + modid + " " + version;

//            auto localMod = modMap_.value(fabricMod.mainId());
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

void LocalModPath::writeToFile()
{
    qDebug() << "write to file";
    QJsonObject object;

    object.insert("latestUpdateCheck", latestUpdateCheck_.toString(Qt::DateFormat::ISODate));

    //mods
    QJsonObject modsObject;
    for(auto mod : qAsConst(modMap_))
        if(mod->modFile())
            modsObject.insert(mod->commonInfo()->id(), mod->toJsonObject());
    object.insert("mods", modsObject);

    if(optiFineMod_)
        object.insert("optifine", optiFineMod_->toJsonObject());

    QJsonDocument doc(object);
    QDir dir(info_.path());
    QFile file(dir.absoluteFilePath(kFileName));
    if(!file.open(QIODevice::WriteOnly)) return;
    file.write(doc.toJson());
    file.close();
}

void LocalModPath::readFromFile()
{
    QDir dir(info_.path());
    QFile file(dir.absoluteFilePath(kFileName));
    if(!file.open(QIODevice::ReadOnly)) return;
    auto bytes = file.readAll();
    file.close();

    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return;
    }
    auto result = jsonDocument.toVariant();

    latestUpdateCheck_ = value(result, "latestUpdateCheck").toDateTime();

    //mods
    auto modMap = value(result, "mods").toMap();
    for(auto it = modMap.cbegin(); it != modMap.cend(); it++)
        if(modMap_.contains(it.key()))
            modMap_[it.key()]->restore(*it);
    if(optiFineMod_ && result.toMap().contains("optifine"))
        optiFineMod_->restore(value(result, "optifine"));
}

bool LocalModPath::isChecking() const
{
    return updateChecker_.isWaiting();
}

bool LocalModPath::isLoading() const
{
    return isLoading_;
}

LocalMod *LocalModPath::optiFineMod() const
{
    return optiFineMod_;
}

void LocalModPath::updateUpdatableCount()
{
    int count = std::count_if(modMap_.cbegin(), modMap_.cend(), [=](LocalMod *mod){
        return !mod->updateTypes().isEmpty();
    });
    for(auto subPath : qAsConst(subPaths_))
        count += subPath->updatableCount();
    if(count == updatableCount_) return;
    updatableCount_ = count;
    emit updatableCountChanged(count);
}

void LocalModPath::onDirectoryChanged(const QString &file)
{
    qDebug() << file;
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

void LocalModPath::linkAllFiles()
{
    if(modMap_.isEmpty() || modsLinker_.isWaiting()) return;
    modsLinker_.start();
    for(auto &&mod : modList()) for(const auto &file : mod->files()){
        auto linker = file->linker();
        modsLinker_.add(linker, &LocalFileLinker::linkStarted, &LocalFileLinker::linkFinished);
        linker->link();
    }
    modsLinker_.done();
}

void LocalModPath::checkModUpdates() // force = true by default
{
    if(modMap_.isEmpty() || updateChecker_.isWaiting()) return;
    updateChecker_.start();
    for(auto &&mod : modList()){
        updateChecker_.add(mod->updateChecker(), &CheckSheet::started, &CheckSheet::finished);
        mod->checkUpdates();
    }
    updateChecker_.done();
}

void LocalModPath::cancelChecking()
{
    updateChecker_.cancel();
    emit checkCancelled();
}

void LocalModPath::updateMods(QList<QPair<LocalMod *, CurseforgeFileInfo> > curseforgeUpdateList,
                              QList<QPair<LocalMod *, ModrinthFileInfo> > modrinthUpdateList)
{
    auto size = curseforgeUpdateList.size() + modrinthUpdateList.size();
    if(!size) return;
    isUpdating_ = true;
    emit updatesStarted();
    auto count = std::make_shared<int>(0);
    auto successCount = std::make_shared<int>(0);
    auto failCount = std::make_shared<int>(0);
    auto bytesReceivedList = std::make_shared<QVector<qint64>>(size);
    auto totalSize = std::make_shared<qint64>(0);

    int i = 0;
    auto updateList = [=, &i](auto &&list){
        for(auto &&[mod, info] : list){
            auto downloader = mod->update(info);
            connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64){
                (*bytesReceivedList)[i] = bytesReceived;
                auto sumReceived = std::accumulate(bytesReceivedList->cbegin(), bytesReceivedList->cend(), 0);
                emit updatesProgress(sumReceived, *totalSize);
            });
            connect(mod, &LocalMod::updateFinished, this, [=](bool success){
                (*count)++;
                if(success)
                    (*successCount)++;
                else
                    (*failCount)++;
                emit updatesDoneCountUpdated(*count, size);
                if(*count == size){
                    isUpdating_ = false;
                    emit updatesDone(*successCount, *failCount);
                }
            });
            i++;
        }
    };
    updateList(curseforgeUpdateList);
    updateList(modrinthUpdateList);
}

QAria2Downloader *LocalModPath::downloadNewMod(DownloadFileInfo &info)
{
    info.setPath(info_.path());
    auto downloader = DownloadManager::manager()->download(info);
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        QFileInfo fileInfo(info_.path(), info.fileName());
        if(!LocalModFile::availableSuffix.contains(fileInfo.suffix())) return;
        auto file = new LocalModFile(this, fileInfo.absoluteFilePath());
        file->loadInfo();
        file->linker()->link();
        addModFile(file);
        if(!file->parent())
            file->deleteLater();
        emit modListUpdated();
    });
    return downloader;
}

const LocalModPathInfo &LocalModPath::info() const
{
    return info_;
}

void LocalModPath::setInfo(const LocalModPathInfo &newInfo, bool deduceLoader)
{
    if(info_ == newInfo) return;

    if(info_.path_ != newInfo.path_){
        watcher_.removePath(info_.path());
        watcher_.addPath(newInfo.path());
    }

    //path, game version or loader type change will trigger mod reload
    if(info_.path() != newInfo.path() || info_.gameVersion() != newInfo.gameVersion() || info_.loaderType() != newInfo.loaderType())
        loadMods(deduceLoader);

    info_ = newInfo;
    emit infoUpdated();
}

Tagable LocalModPath::containedTags()
{
    return containedTags_;
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

QList<LocalMod *> LocalModPath::modList() const
{
    QList<LocalMod *> list;
    for(auto mod : modMap_){
        if(mod->modFile())
            list << mod;
    }
    if(optiFineMod_ && optiFineMod_->modFile())
        list << optiFineMod_;
    for(auto &&path : subPaths_)
        list << path->modList();
    return list;
}

void LocalModPath::deleteAllOld() const
{
    for(auto mod : modMap_)
        mod->deleteAllOld();
    //TODO: inform finished
}
