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
    connect(this, &LocalFileLinker::linkCurseforgeFinished, this, [=](bool success, int id){
        curseforgeLinked_ = true;
        if(success && id && localFile_->mod() && !localFile_->mod()->curseforgeMod())
            localFile_->mod()->setCurseforgeId(id);
    });
    connect(this, &LocalFileLinker::linkModrinthFinished, this, [=](bool success, QString id){
        modrinthLinked_ = true;
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
    emit linkStarted();
    auto count = std::make_shared<int>(2);
    auto hasWeb = std::make_shared<bool>(false);
    auto foo = [=](bool bl){
        if(bl) *hasWeb = true;
        if(--(*count) == 0) emit linkFinished(*hasWeb);
    };
    connect(this, &LocalFileLinker::linkCurseforgeFinished, this, foo);
    connect(this, &LocalFileLinker::linkModrinthFinished, this, foo);
    linkCurseforge();
    linkModrinth();
}

void LocalFileLinker::linkCurseforge()
{
    emit linkCurseforgeStarted();
    auto &&murmurhash = localFile_->murmurhash();
    //file cache
    if(KnownFile::curseforgeFiles().keys().contains(murmurhash)){
        setCurseforgeFileInfo(KnownFile::curseforgeFiles().value(murmurhash));
        emit linkCurseforgeFinished(true);
        return;
    }
    if(KnownFile::unmatchedCurseforgeFiles().contains(murmurhash)){
        emit linkCurseforgeFinished(false);
        return;
    }
    qDebug() << "try link curseforge:" << murmurhash;
    CurseforgeAPI *api;
    if(localFile_->modPath())
        api = localFile_->modPath()->curseforgeAPI();
    else
        api = CurseforgeAPI::api();
    connect(this, &QObject::destroyed, disconnecter(
                api->getIdByFingerprint(murmurhash, [=](int id, auto fileInfo, const QList<CurseforgeFileInfo> &fileList[[maybe_unused]]){
        IdMapper::addCurseforge(localFile_->commonInfo()->id(), id);
        qDebug() << "success link curseforge:" << murmurhash;
        KnownFile::addCurseforge(murmurhash, fileInfo);
        emit linkCurseforgeFinished(true, id);
    }, [=]{
        qDebug() << "fail link curseforge:" << murmurhash;
        KnownFile::addUnmatchedCurseforge(murmurhash);
        emit linkCurseforgeFinished(false);
    })));
}

void LocalFileLinker::linkModrinth()
{
    emit linkModrinthStarted();
    auto &&sha1 = localFile_->sha1();
    //file cache
    if(KnownFile::modrinthFiles().keys().contains(sha1)){
        setModrinthFileInfo(KnownFile::modrinthFiles().value(sha1));
        emit linkModrinthFinished(true);
        return;
    }
    if(KnownFile::unmatchedModrinthFiles().contains(sha1)){
        emit linkModrinthFinished(false);
        return;
    }
    qDebug() << "try link modrinth:" << sha1;
    ModrinthAPI *api;
    if(localFile_->modPath())
        api = localFile_->modPath()->modrinthAPI();
    else
        api = ModrinthAPI::api();
    connect(this, &QObject::destroyed, disconnecter(
                api->getVersionFileBySha1(sha1, [=](const ModrinthFileInfo &fileInfo){
        IdMapper::addModrinth(localFile_->commonInfo()->id(), fileInfo.modId());
        KnownFile::addModrinth(sha1, fileInfo);
        qDebug() << "success link modrinth:" << sha1;
        emit linkModrinthFinished(true, fileInfo.modId());
    }, [=]{
        qDebug() << "fail link modrinth:" << sha1;
        KnownFile::addUnmatchedModrinth(sha1);
        emit linkModrinthFinished(false);
    })));
}

std::optional<CurseforgeFileInfo> LocalFileLinker::curseforgeFileInfo() const
{
    return curseforgeFileInfo_;
}

void LocalFileLinker::setCurseforgeFileInfo(CurseforgeFileInfo newCurseforgeFileInfo)
{
    curseforgeFileInfo_ = newCurseforgeFileInfo;
    emit curseforgeFileInfoChanged(curseforgeFileInfo_);
}

std::optional<ModrinthFileInfo> LocalFileLinker::modrinthFileInfo() const
{
    return modrinthFileInfo_;
}

void LocalFileLinker::setModrinthFileInfo(ModrinthFileInfo newModrinthFileInfo)
{
    modrinthFileInfo_ = newModrinthFileInfo;
    emit modrinthFileInfoChanged(modrinthFileInfo_);
}
