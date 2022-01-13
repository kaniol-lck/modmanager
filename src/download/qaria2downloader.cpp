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
    if(!handle_) handle_ = aria2::getDownloadHandle(QAria2::qaria2()->session(), gid_);
    if(!handle_) return;
    if(status_ != aria2::DOWNLOAD_ACTIVE) return;
    if(auto files = handle_->getFiles(); !files.empty()){
        auto &&file = files.front();
        if(QFileInfo path = QString::fromStdString(file.path); path.isFile()){
            info_.setFileName(path.fileName());
            emit infoChanged();
        }
    }
    emit downloadProgress(handle_->getCompletedLength(), handle_->getTotalLength());
    emit downloadSpeed(handle_->getDownloadSpeed(), handle_->getUploadSpeed());
}

aria2::DownloadStatus QAria2Downloader::status() const
{
    return status_;
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

void QAria2Downloader::setGid(aria2::A2Gid newGid)
{
    gid_ = newGid;
}

void QAria2Downloader::pause()
{
    if(status_ != aria2::DOWNLOAD_ACTIVE) return;
    pauseDownload(QAria2::qaria2()->session(), gid_);
}

void QAria2Downloader::start()
{
    if(status_ != aria2::DOWNLOAD_PAUSED) return;
    unpauseDownload(QAria2::qaria2()->session(), gid_);
}
