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
{}

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
        setCurseforgeFile(KnownFile::curseforgeFiles().value(murmurhash));
        emit linkCurseforgeFinished(true);
        return;
    }
    if(KnownFile::unmatchedCurseforgeFiles().contains(murmurhash)){
        emit linkCurseforgeFinished(false);
        return;
    }
    qDebug() << "link curseforge:" << murmurhash;
    connect(this, &QObject::destroyed, disconnecter(
                CurseforgeAPI::api()->getIdByFingerprint(murmurhash, [=](int id, auto fileInfo, const QList<CurseforgeFileInfo> &fileList[[maybe_unused]]){
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
        setModrinthFile(KnownFile::modrinthFiles().value(sha1));
        emit linkModrinthFinished(true);
        return;
    }
    if(KnownFile::unmatchedModrinthFiles().contains(sha1)){
        emit linkModrinthFinished(false);
        return;
    }
    qDebug() << "link modrinth:" << sha1;
    connect(this, &QObject::destroyed, disconnecter(
                ModrinthAPI::api()->getVersionFileBySha1(sha1, [=](const ModrinthFileInfo &fileInfo){
        IdMapper::addModrinth(localFile_->commonInfo()->id(), fileInfo.modId());
        KnownFile::addModrinth(sha1, fileInfo);
        emit linkModrinthFinished(true);
    }, [=]{
        KnownFile::addUnmatchedModrinth(sha1);
        emit linkModrinthFinished(false);
    })));
}

std::optional<CurseforgeFileInfo> LocalFileLinker::curseforgeFile() const
{
    return curseforgeFile_;
}

void LocalFileLinker::setCurseforgeFile(CurseforgeFileInfo newCurseforgeFile)
{
    curseforgeFile_ = newCurseforgeFile;
    emit curseforgeFileChanged(curseforgeFile_);
}

std::optional<ModrinthFileInfo> LocalFileLinker::modrinthFile() const
{
    return modrinthFile_;
}

void LocalFileLinker::setModrinthFile(ModrinthFileInfo newModrinthFile)
{
    modrinthFile_ = newModrinthFile;
    emit modrinthFileChanged(modrinthFile_);
}
