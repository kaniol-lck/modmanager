#include "localfilelinker.h"

#include "localmodpath.h"
#include "localmodfile.h"
#include "knownfile.h"
#include "idmapper.h"
#include "curseforge/curseforgeapi.h"
#include "modrinth/modrinthapi.h"
#include "util/funcutil.h"

LocalFileLinker::LocalFileLinker(LocalModFile *localFile) :
    QObject(localFile),
    localFile_(localFile)
{
    //TODO: update mod info
    connect(this, &LocalFileLinker::curseforgeFileInfoChanged, localFile_, &LocalModFile::fileChanged);
    connect(this, &LocalFileLinker::modrinthFileInfoChanged, localFile_, &LocalModFile::fileChanged);
    // 这里刻意不置 linked 标志位。
    // 原来无论 success 与否都置位，于是"一次网络抖动"就被当成"已链接"：
    // link() 从此早退，本会话内再也无法重试；LocalMod::checkUpdates 也会因为
    // curseforgeFileInfo() 为空而静默跳过这个 mod 的 CF 更新检查。
    // 标志位只在两种确定的情况下置位（成功，或确定"无匹配"）—— 见下面各函数。
    connect(this, &LocalFileLinker::linkCurseforgeFinished, this, [=](bool success, int id){
        if(success && id && localFile_->mod() && !localFile_->mod()->curseforgeMod())
            localFile_->mod()->setCurseforgeId(id);
    });
    connect(this, &LocalFileLinker::linkModrinthFinished, this, [=](bool success, QString id){
        if(success && !id.isEmpty() && localFile_->mod() && !localFile_->mod()->modrinthMod())
            localFile_->mod()->setModrinthId(id);
    });
}

bool LocalFileLinker::linked() const
{
    return curseforgeLinked_  && modrinthLinked_;
}

void LocalFileLinker::link()
{
    if(localFile_->loaderTypes().isEmpty()) return;
    if(curseforgeLinked_  && modrinthLinked_) return;
    // 同一个 linker 只允许一轮在途请求。重复 link() 会对同一个文件发两遍请求，
    // 两轮的 linkFinished 会把 CheckSheet 的计数彻底打乱（提前 finished）。
    if(isLinking_) return;
    isLinking_ = true;
    emit linkStarted();
    auto count = std::make_shared<int>(0);
    auto hasWeb = std::make_shared<bool>(false);
    auto foo = [=](bool bl){
        if(bl) *hasWeb = true;
        if(--(*count) == 0){
            isLinking_ = false;
            emit linkFinished(*hasWeb);
        }
    };
    if(!curseforgeLinked_){
        (*count) ++;
        connect(this, &LocalFileLinker::linkCurseforgeFinished, this, foo);
        linkCurseforge();
    }
    if(!modrinthLinked_){
        (*count) ++;
        connect(this, &LocalFileLinker::linkModrinthFinished, this, foo);
        linkModrinth();
    }
}

void LocalFileLinker::linkCached()
{
    linkCachedCurseforge();
    linkCachedModrinth();
}

void LocalFileLinker::linkCachedCurseforge()
{
    auto &&murmurhash = localFile_->murmurhash();
    if(KnownFile::curseforgeFiles().keys().contains(murmurhash)){
        setCurseforgeFileInfo(KnownFile::curseforgeFiles().value(murmurhash));
        curseforgeLinked_ = true;
        return;
    }
    if(KnownFile::unmatchedCurseforgeFiles().contains(murmurhash)){
        curseforgeLinked_ = true;
        return;
    }
}

void LocalFileLinker::linkCurseforge()
{
    emit linkCurseforgeStarted();
    auto &&murmurhash = localFile_->murmurhash();
    qDebug() << "try link curseforge:" << murmurhash << localFile_->fileInfo().fileName();
    CurseforgeAPI *api;
    if(localFile_->modPath())
        api = localFile_->modPath()->curseforgeAPI();
    else
        api = CurseforgeAPI::api();
    curseforgeSearcher_ = api->getIdByFingerprint(murmurhash).asUnique();
    curseforgeSearcher_->setOnFinished(this, [=](int id, auto fileInfo, const QList<CurseforgeFileInfo> &fileList[[maybe_unused]]){
        if(id){
            setCurseforgeFileInfo(fileInfo);
            IdMapper::addCurseforge(localFile_->commonInfo()->id(), id);
            qDebug() << "success link curseforge:" << murmurhash;
            KnownFile::addCurseforge(murmurhash, fileInfo);
            curseforgeLinked_ = true;
            emit linkCurseforgeFinished(true, id);
        } else{
            qDebug() << "fail link curseforge:" << murmurhash;
            KnownFile::addUnmatchedCurseforge(murmurhash);
            // 确定"无匹配"（不是请求失败）：本会话内不再重试，缓存里也记下了
            curseforgeLinked_ = true;
            emit linkCurseforgeFinished(false);
        }
    }, [=](auto){
        // 请求失败（网络错误等）：不置位、不写 unmatched 缓存，
        // 这样重新 link / 下次加载时还能再试一次
        emit linkCurseforgeFinished(false);
    });
}

void LocalFileLinker::linkCachedModrinth()
{
    auto &&sha1 = localFile_->sha1();
    if(KnownFile::modrinthFiles().keys().contains(sha1)){
        setModrinthFileInfo(KnownFile::modrinthFiles().value(sha1));
        modrinthLinked_ = true;
        return;
    }
    if(KnownFile::unmatchedModrinthFiles().contains(sha1)){
        modrinthLinked_ = true;
        return;
    }
}

void LocalFileLinker::linkModrinth()
{
    emit linkModrinthStarted();
    auto &&sha1 = localFile_->sha1();
    qDebug() << "try link modrinth:" << sha1 << localFile_->fileInfo().fileName();
    ModrinthAPI *api;
    if(localFile_->modPath())
        api = localFile_->modPath()->modrinthAPI();
    else
        api = ModrinthAPI::api();
    modrinthSearcher_ = api->getVersionFileBySha1(sha1).asUnique();
    modrinthSearcher_->setOnFinished(this, [=](const ModrinthFileInfo &fileInfo){
        setModrinthFileInfo(fileInfo);
        IdMapper::addModrinth(localFile_->commonInfo()->id(), fileInfo.modId());
        KnownFile::addModrinth(sha1, fileInfo);
        qDebug() << "success link modrinth:" << sha1;
        modrinthLinked_ = true;
        emit linkModrinthFinished(true, fileInfo.modId());
    }, [=](const auto &error){
        if(error == QNetworkReply::ContentNotFoundError){
            qDebug() << "fail link modrinth:" << sha1;
            KnownFile::addUnmatchedModrinth(sha1);
            modrinthLinked_ = true;      // 确定无匹配，本会话内不再重试
        }
        // 其它错误（网络等）不置位，留给下一次重试
        emit linkModrinthFinished(false);
    });
}

std::optional<CurseforgeFileInfo> LocalFileLinker::curseforgeFileInfo() const
{
    return curseforgeFileInfo_;
}

void LocalFileLinker::setCurseforgeFileInfo(CurseforgeFileInfo newCurseforgeFileInfo)
{
    curseforgeFileInfo_ = newCurseforgeFileInfo;
    emit curseforgeFileInfoChanged();
}

std::optional<ModrinthFileInfo> LocalFileLinker::modrinthFileInfo() const
{
    return modrinthFileInfo_;
}

void LocalFileLinker::setModrinthFileInfo(ModrinthFileInfo newModrinthFileInfo)
{
    modrinthFileInfo_ = newModrinthFileInfo;
    emit modrinthFileInfoChanged();
}
