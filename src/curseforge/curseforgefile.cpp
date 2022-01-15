#include "curseforgefile.h"

#include "curseforge/curseforgeapi.h"

CurseforgeFile::CurseforgeFile(QObject *parent, int projectID, int fileID) :
    QObject(parent),
    api_(CurseforgeAPI::api()),
    projectID_(projectID),
    info_(fileID)
{}

std::shared_ptr<Reply<CurseforgeFileInfo> > CurseforgeFile::acquireFileInfo()
{
    if(infoGetter_ && infoGetter_->isRunning()) return {};
    infoGetter_ = api_->getFileInfo(projectID_, info_.id()).asShared();
    infoGetter_->setOnFinished(this, [=](const CurseforgeFileInfo &fileInfo){
        info_ = fileInfo;
        emit infoReady();
    }, [=](auto){
        emit infoReady();
    });
    return infoGetter_;
}

const CurseforgeFileInfo &CurseforgeFile::info() const
{
    return info_;
}
