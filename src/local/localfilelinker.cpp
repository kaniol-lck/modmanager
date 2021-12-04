#include "localfilelinker.h"

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
    connect(this, &LocalFileLinker::linkCurseforgeFinished, this, [=]{
        curseforgeLinked_ = true;
    });
    connect(this, &LocalFileLinker::linkModrinthFinished, this, [=]{
        modrinthLinked_ = true;
    });
}

bool LocalFileLinker::linked() const
{
    return curseforgeLinked_  && modrinthLinked_;
}

void LocalFileLinker::link()
{
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
    qDebug() << "link curseforge:" << murmurhash;
    static CurseforgeAPI api;
    connect(this, &QObject::destroyed, disconnecter(
                api.getIdByFingerprint(murmurhash, [=](int id, auto fileInfo, const QList<CurseforgeFileInfo> &fileList[[maybe_unused]]){
        IdMapper::addCurseforge(localFile_->commonInfo()->id(), id);
        KnownFile::addCurseforge(murmurhash, fileInfo);
        emit linkCurseforgeFinished(true);
    }, [=]{
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
    qDebug() << "link modrinth:" << sha1;
    static ModrinthAPI api;
    connect(this, &QObject::destroyed, disconnecter(
                api.getVersionFileBySha1(sha1, [=](const ModrinthFileInfo &fileInfo){
        IdMapper::addModrinth(localFile_->commonInfo()->id(), fileInfo.modId());
        KnownFile::addModrinth(sha1, fileInfo);
        emit linkModrinthFinished(true);
    }, [=]{
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
