#include "qaria2downloader.h"

#include <QDebug>
#include <QDir>

#include "qaria2.h"
#include "util/mmlogger.h"

QAria2Downloader::QAria2Downloader(aria2::A2Gid gid) :
    AbstractDownloader(QAria2::qaria2()),
    gid_(gid)
{}

QAria2Downloader::QAria2Downloader(const DownloadFileInfo &info) :
    AbstractDownloader(QAria2::qaria2(), info)
{}

void QAria2Downloader::deleteDownloadHandle()
{
    aria2::deleteDownloadHandle(handle_);
    handle_ = nullptr;
}

void QAria2Downloader::update()
{
    handle_ = aria2::getDownloadHandle(QAria2::qaria2()->session(), gid_);
    emit handleUpdated();
//    if(!handle_) return;
//    if(status_ != aria2::DOWNLOAD_ACTIVE) return;
    if(auto files = handle_->getFiles(); !files.empty()){
        auto &&file = files.front();
        if(QFileInfo path = QString::fromStdString(file.path); path.isFile()){
            info_.setFileName(path.fileName());
            emit infoChanged();
        }
    }
    emit downloadProgress(handle_->getCompletedLength(), handle_->getTotalLength());
    emit downloadSpeed(handle_->getDownloadSpeed(), handle_->getUploadSpeed());
    qDebug() << QString::fromStdString(aria2::gidToHex(gid_))
             << handle_->getCompletedLength() << handle_->getTotalLength()
             << handle_->getDownloadSpeed() << handle_->getUploadSpeed()
             << handle_->getConnections() << handle_->getNumPieces()
             << handle_->getPieceLength() << handle_->getStatus();
}

aria2::DownloadStatus QAria2Downloader::status() const
{
    return status_;
}

bool QAria2Downloader::isStarted() const
{
    return status_ == aria2::DOWNLOAD_ACTIVE;
}

bool QAria2Downloader::isPaused() const
{
    return status_ == aria2::DOWNLOAD_PAUSED;
}

bool QAria2Downloader::isStopped() const
{
    return status_ == aria2::DOWNLOAD_REMOVED;
}

void QAria2Downloader::setEvent(const aria2::DownloadEvent &event)
{
    qDebug() << gid_ << "event:" << event;
    switch (event) {
    case aria2::EVENT_ON_DOWNLOAD_START:
        status_ = aria2::DOWNLOAD_ACTIVE;
        break;
    case aria2::EVENT_ON_DOWNLOAD_PAUSE:
        status_ = aria2::DOWNLOAD_PAUSED;
        break;
    case aria2::EVENT_ON_DOWNLOAD_STOP:
        status_ = aria2::DOWNLOAD_REMOVED;
        break;
    case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
        status_ = aria2::DOWNLOAD_COMPLETE;
        update();
        emit finished();
        break;
    case aria2::EVENT_ON_DOWNLOAD_ERROR:
        status_ = aria2::DOWNLOAD_ERROR;
        MMLogger("Download", "error") << handle_->getErrorCode();
        break;
    case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
        status_ = aria2::DOWNLOAD_COMPLETE;
        break;
    }
    emit statusChanged(status_);
}

aria2::DownloadHandle *QAria2Downloader::handle() const
{
    return handle_;
}

void QAria2Downloader::setGid(aria2::A2Gid newGid)
{
    gid_ = newGid;
}

int QAria2Downloader::pause(bool force)
{
    if(status_ != aria2::DOWNLOAD_ACTIVE) return -1;
    return pauseDownload(QAria2::qaria2()->session(), gid_, force);
}

int QAria2Downloader::start()
{
    if(status_ != aria2::DOWNLOAD_PAUSED) return -1;
    return unpauseDownload(QAria2::qaria2()->session(), gid_);
}

int QAria2Downloader::stop(bool force)
{
    return removeDownload(QAria2::qaria2()->session(), gid_, force);
}
