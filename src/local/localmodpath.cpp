#include "localmodpath.h"

#include <QDir>
#include <QFuture>
#include <QSet>
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
    connect(this, &LocalModPath::loadFinished, this, &LocalModPath::autoCheckWhenIdle);
    // 这个连接只注册一次。原来是写在 loadFinished 的 lambda 里，reload N 次就多出 N 条，
    // 一次检查完成会重复写 N 遍 mods.json。
    connect(&updateChecker_, &CheckSheet::finished, this, [=]{
        latestUpdateCheck_ = QDateTime::currentDateTime();
        writeToFile();
    });
//        connect(&modsLinker_, &CheckSheet::finished, this, &LocalModPath::updatesReady);
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
    // 子路径自己不触发自动链接/检查，统一交给最外层路径（见 autoCheckWhenIdle）；
    // 但要在最外层路径空闲时把它"唤醒"，因为我们可能比父路径更晚才扫描完。
    connect(this, &LocalModPath::loadFinished, rootPath(), &LocalModPath::autoCheckWhenIdle);
    // 可更新计数逐层向上传播，否则只有子目录里有可更新项时顶层 UI 永远不显示 "N mods need update"
    connect(this, &LocalModPath::updatableCountChanged, path, &LocalModPath::updateUpdatableCount);
}

LocalModPath *LocalModPath::rootPath() const
{
    // 根路径的 parent 是 LocalModPathManager，不是 LocalModPath
    if(auto path = qobject_cast<LocalModPath *>(parent()))
        return path->rootPath();
    return const_cast<LocalModPath *>(this);
}

void LocalModPath::autoCheckWhenIdle()
{
    // 自动链接 + 自动检查更新只在最外层路径上跑一次。
    // modList() 已经递归包含子路径的 mod，父子各跑一遍会让同一个 linker / mod
    // 被两个 CheckSheet 各注册一次：网络请求翻倍、计数语义被打乱，
    // 还可能提前 finished —— 而提前 finished 会让一部分 mod 还没 link 完就开始检查更新，
    // 那些 mod 因为 curseforgeFileInfo() 还是空的而被静默跳过。
    if(auto *root = rootPath(); root != this){
        root->autoCheckWhenIdle();
        return;
    }

    if(isLoading() || modsLinker_.isWaiting()) return;
    for(auto &&subPath : qAsConst(subPaths_))
        if(subPath->isLoading()) return;    // 子路径还在扫描，它完成时会再次调用本函数
    if(modList().isEmpty()) return;

    auto conn = connect(&modsLinker_, &CheckSheet::finished, this, [=]{
        auto interval = Config().getUpdateCheckInterval();
        if(interval == Config::Always || !latestUpdateCheck_.isValid() ||
                (interval == Config::EveryDay && latestUpdateCheck_.daysTo(QDateTime::currentDateTime()) >= 1))
            checkModUpdates();
    });
    qDebug() << "load finished";
    linkAllFiles();
    connect(&modsLinker_, &CheckSheet::finished, this, disconnecter(conn));
}

const QStringList &LocalModPath::nonModFiles() const
{
    return nonModFiles_;
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
    loaded_ = true;
    isLoading_ = true;
    modsLinker_.reset();
    updateChecker_.reset();
    isUpdating_ = false;
    QDir dir(info_.path());
    QSet<QString> existingSubDirs;
    for(auto &&fileInfo : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)){
        bool containsMod = false;
        for(auto &&fileInfo2 : fileInfo.dir().entryInfoList(QDir::Files))
            if((containsMod = LocalModFile::availableSuffix.contains(fileInfo2.suffix())))
                break;
        if(!containsMod) continue;
        auto fileName = fileInfo.fileName();
        existingSubDirs << fileName;
        if(auto existingSubPath = subPaths_.value(fileName)){
            // 已存在的子路径也必须重扫：只在"首次发现"时 load 的话，
            // 子目录里手工增删的 jar 在 Refresh 之后不会生效。
            existingSubPath->loadMods();
        } else {
            auto subPath = new LocalModPath(this, fileName);
            subPaths_.insert(fileName, subPath);
            containedTags_.addSubTagable(subPath);
            containedTags_.addSubTagable(&subPath->containedTags_);
            subPath->loadMods();
        }
    }
    // 被删除/改名的子目录要从 subPaths_ 里摘掉，
    // 否则它的旧内容会一直挂在 modList() 里显示，标签筛选里也会留下已不存在的目录标签。
    for(auto it = subPaths_.begin(); it != subPaths_.end();){
        auto subPath = it.value();
        // 正在扫描的子路径不能删：它的 QtConcurrent 任务还持有 this，
        // 这里 deleteLater 会让另一个线程访问已析构对象。留到下一轮刷新再清理。
        if(existingSubDirs.contains(it.key()) || subPath->isLoading()){
            ++it;
            continue;
        }
        containedTags_.removeSubTagable(subPath);
        containedTags_.removeSubTagable(&subPath->containedTags_);
        it = subPaths_.erase(it);
        subPath->setParent(nullptr);
        subPath->deleteLater();
    }
    QList<LocalModFile*> modFileList;
    for(auto &&fileInfo : dir.entryInfoList(QDir::Files))
        if(LocalModFile::availableSuffix.contains(fileInfo.suffix()))
            modFileList << new LocalModFile(this, fileInfo.absoluteFilePath(), relative_);

    auto future = QtConcurrent::run([=]{
        int count = 0;
        QMap<ModLoaderType::Type, int> modCount;
        emit loadStarted();
        for(auto &file : modFileList){
            modCount[file->loadInfo()]++;
            emit loadProgress(++count, modFileList.size());
        }
        if(autoLoaderType){
            if(auto iter = std::max_element(modCount.cbegin(), modCount.cend()); iter != modCount.cend()){
                auto loaderType = iter.key();
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
        nonModFiles_.clear();
        for(const auto &file : qAsConst(modFileList)){
            if(file->parent() == this){
                nonModFiles_ << file->fileInfo().fileName();
                file->deleteLater();
            }
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
        connect(mod, &LocalMod::updateReady, this, &LocalModPath::updateUpdatableCount);
        connect(mod, &LocalMod::updateFinished, this, &LocalModPath::updateUpdatableCount);
        modMap_[id] = mod;
        containedTags_.addSubTagable(mod);
    }
}

void LocalModPath::removeModFile(LocalModFile *file)
{
    // 注意：这里不能 deleteLater 那个被掏空的 LocalMod —— 调用方（LocalModFile::moveTo）
    // 是先 removeModFile(this) 再 path->addModFile(this)，此刻文件还是旧 mod 的子对象，
    // 删掉 mod 会连带删掉正在被移动的文件。只把它从标签集合里摘掉就够了。
    for(const auto &mod : qAsConst(modMap_))
        if(mod->files().contains(file)){
            mod->removeModFile(file);
            if(!mod->modFile()){
                modMap_.remove(file->commonInfo()->id());
                containedTags_.removeSubTagable(mod);
            }
            return;
        }
    if(optiFineMod_ && optiFineMod_->files().contains(file)){
        optiFineMod_->removeModFile(file);
        if(!optiFineMod_->modFile()){
            containedTags_.removeSubTagable(optiFineMod_);
            optiFineMod_ = nullptr;
        }
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
    int count = std::count_if(modMap_.cbegin(), modMap_.cend(), [](LocalMod *mod){
        return !mod->updateTypes().isEmpty();
    });
    // OptiFine 单独挂在 optiFineMod_ 上，不在 modMap_ 里；
    // 漏掉它会让"只有 optifine 有更新"时计数恒为 0。
    if(optiFineMod_ && !optiFineMod_->updateTypes().isEmpty())
        count++;
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
    // 守卫必须基于 modList()（递归含子路径），不能用 modMap_：
    // 主 mods 目录里只放目录结构、jar 全在子目录时 modMap_ 是空的，
    // 用 modMap_ 会让工具栏的 Link / Check 变成空操作。
    if(modsLinker_.isWaiting()) return;
    auto mods = modList();
    if(mods.isEmpty()) return;
    modsLinker_.start();
    for(auto &&mod : mods) for(const auto &file : mod->files()){
        auto linker = file->linker();
        modsLinker_.add(linker, &LocalFileLinker::linkStarted, &LocalFileLinker::linkFinished);
        linker->link();
    }
    modsLinker_.done();
}

void LocalModPath::checkModUpdates() // force = true by default
{
    if(updateChecker_.isWaiting()) return;
    auto mods = modList();
    if(mods.isEmpty()) return;
    updateChecker_.start();
    for(auto &&mod : mods){
        updateChecker_.add(mod, &LocalMod::checkUpdateStarted, &LocalMod::checkUpdateFinished);
        mod->checkUpdates();
    }
    updateChecker_.done();
}

void LocalModPath::cancelChecking()
{
    for(auto &&mod : modList())
        mod->cancelChecking();
    updateChecker_.reset();
    emit updateChecker_.finished();
    emit checkCancelled();
}

void LocalModPath::updateMods(QList<QPair<LocalMod *, CurseforgeFileInfo> > curseforgeUpdateList,
                              QList<QPair<LocalMod *, ModrinthFileInfo> > modrinthUpdateList)
{
    auto size = curseforgeUpdateList.size() + modrinthUpdateList.size();
    if(!size) return;
    // 一批还没跑完就不要再接一批：否则两批共用同一批 mod 的信号，
    // 计数会互相干扰（上一批的闭包被重新触发，updatesDone 提前或重复发射）。
    if(isUpdating_) return;
    isUpdating_ = true;
    emit updatesStarted();
    auto count = std::make_shared<int>(0);
    auto successCount = std::make_shared<int>(0);
    auto failCount = std::make_shared<int>(0);
    auto bytesReceivedList = std::make_shared<QVector<qint64>>(size);
    auto totalSize = std::make_shared<qint64>(0);
    // 本批建立的连接要在批次结束时统一断开。原来这些连接建在 mod 上、从不清理，
    // 第二次 Update All 时第一批遗留的闭包会再次触发（捕获的还是上一批的计数）。
    auto connections = std::make_shared<QList<QMetaObject::Connection>>();

    int i = 0;
    auto updateList = [=, &i](auto &&list){
        for(auto &&[mod, info] : list){
            auto downloader = mod->update(info);
            connections->append(connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64){
                (*bytesReceivedList)[i] = bytesReceived;
                auto sumReceived = std::accumulate(bytesReceivedList->cbegin(), bytesReceivedList->cend(), 0);
                emit updatesProgress(sumReceived, *totalSize);
            }));
            connections->append(connect(mod, &LocalMod::updateFinished, this, [=](bool success){
                (*count)++;
                if(success)
                    (*successCount)++;
                else
                    (*failCount)++;
                emit updatesDoneCountUpdated(*count, size);
                if(*count == size){
                    isUpdating_ = false;
                    for(auto &&conn : *connections)
                        QObject::disconnect(conn);
                    connections->clear();
                    emit updatesDone(*successCount, *failCount);
                }
            }));
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

QAria2Downloader *LocalModPath::downloadNewMod(CurseforgeMod *mod, CurseforgeFile *file)
{
    auto downloader = DownloadManager::manager()->download(mod, file, info_.name(), info_.path());
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        QFileInfo fileInfo(info_.path(), downloader->info().fileName());
        if(!LocalModFile::availableSuffix.contains(fileInfo.suffix())) return;
        auto file = new LocalModFile(this, fileInfo.absoluteFilePath());
        auto future = QtConcurrent::run([=]{
            file->loadInfo();
        });
        auto watcher = new QFutureWatcher<void>(this);
        watcher->setFuture(future);
        connect(watcher, &QFutureWatcher<void>::finished, this, [=]{
            file->linker()->link();
            addModFile(file);
            if(!file->parent())
                file->deleteLater();
            emit modListUpdated();
        });
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
    if(info_.path() != newInfo.path() || info_.gameVersion() != newInfo.gameVersion() || info_.loaderType() != newInfo.loaderType()){
        // info_ 必须在 loadMods() 之前更新：loadMods 用的是 info_.path() 去扫描目录。
        // 原来放在 loadMods 之后，改完路径扫的还是旧目录（异步段又用新 info 过滤），
        // 于是必须再手动 Reload 一次才对。
        info_ = newInfo;
        loadMods(deduceLoader);
    } else
        info_ = newInfo;

    emit infoUpdated();
}

Tagable &LocalModPath::containedTags()
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
