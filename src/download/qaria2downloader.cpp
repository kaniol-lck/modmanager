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

QAria2Downloader::~QAria2Downloader()
{
    // aria2 的 deleteDownloadHandle 明确允许传 nullptr
    aria2::deleteDownloadHandle(handle_);
    handle_ = nullptr;
}

void QAria2Downloader::deleteDownloadHandle()
{
    aria2::deleteDownloadHandle(handle_);
    handle_ = nullptr;
}

void QAria2Downloader::reportFailure()
{
    QMetaObject::invokeMethod(this, [this]{
        setEvent(aria2::EVENT_ON_DOWNLOAD_ERROR);
    }, Qt::QueuedConnection);
}

void QAria2Downloader::update()
{
    // gid 为 0 表示这个任务从未成功交给 aria2（addUri 失败），不会有任何句柄
    if(!gid_) return;
    // aria2::getDownloadHandle 每次都会新建一个 DownloadHandle，
    // 原来每次 update()（每秒一次）都把上一个直接覆盖掉 —— 每个在跑的任务每秒漏一个句柄。
    if(handle_)
        aria2::deleteDownloadHandle(handle_);
    handle_ = aria2::getDownloadHandle(QAria2::qaria2()->session(), gid_);
    emit handleUpdated();
    // 任务已被移除（或句柄已释放）时返回 nullptr，
    // 原实现把这个判断注释掉了，紧跟着就是 handle_->getFiles() 解引用。
    if(!handle_){
        // 句柄突然取不到，说明这个任务已经不在 aria2 的会话里了
        // （removeDownload 之后、或事件丢失后任务被回收）。
        // 原来的写法是直接 return：任务再也不会结束，却会被每秒轮询到天荒地老，
        // 等它的人（更新流程 / 进度条）也永远等不到结果。
        // 连续几次都取不到就按失败收场；单次取不到可能只是瞬时状态，先让过去。
        if(++nullHandleTicks_ >= 3){
            status_ = aria2::DOWNLOAD_ERROR;
            stopSpeedTimer();
            reportResult(false);
            emit statusChanged(status_);
        }
        return;
    }
    nullHandleTicks_ = 0;

    if(auto files = handle_->getFiles(); !files.empty()){
        auto &&file = files.front();
        if(QFileInfo path(QString::fromStdString(file.path)); path.isFile()){
            info_.setFileName(path.fileName());
            emit infoChanged();
        }
    }
    // 顺带校正一次状态：事件偶尔会丢（比如任务在 addUri 之后立刻失败），
    // 这里能从句柄拿到权威状态，避免 UI 一直显示"等待中"。
    if(auto status = handle_->getStatus(); status != status_){
        status_ = status;
        if(isTerminal())
            stopSpeedTimer();
        emit statusChanged(status_);
    }
    emit downloadProgress(handle_->getCompletedLength(), handle_->getTotalLength());
    emit downloadSpeed(handle_->getDownloadSpeed(), handle_->getUploadSpeed());
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

bool QAria2Downloader::isTerminal() const
{
    return status_ == aria2::DOWNLOAD_COMPLETE
            || status_ == aria2::DOWNLOAD_ERROR
            || status_ == aria2::DOWNLOAD_REMOVED;
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
        stopSpeedTimer();
        // 任务被取消（用户点 Stop，或应用退出时 aria2 清理）也必须报告一次，
        // 否则等它的人（更新流程的计数）永远等不到结果。已完成的任务不会走到这里
        // （resultReported_ 已经置位），所以不会把"成功"改报成"失败"。
        reportResult(false);
        break;
    case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
        status_ = aria2::DOWNLOAD_COMPLETE;
        update();
        stopSpeedTimer();
        reportResult(true);
        break;
    case aria2::EVENT_ON_DOWNLOAD_ERROR:
        status_ = aria2::DOWNLOAD_ERROR;
        if(handle_)
            MMLogger("Download", "error") << handle_->getErrorCode();
        else
            MMLogger("Download", "error") << "download failed before any handle was available";
        stopSpeedTimer();
        reportResult(false);
        break;
    case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
        status_ = aria2::DOWNLOAD_COMPLETE;
        reportResult(true);
        break;
    }
    emit statusChanged(status_);
}

// 一个下载器只允许报告一次终态。重复报告会把已经"更新成功"的条目又改回"重试更新"，
// 也会让 LocalModPath::updateMods 的计数被多算一次。
void QAria2Downloader::reportResult(bool success)
{
    if(resultReported_) return;
    resultReported_ = true;
    if(success)
        emit finished();
    else
        emit downloadFailed();
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
    if(!gid_) return -1;
    // WAITING 也要允许暂停：并发上限之外排队中的任务原来根本停不下来
    if(status_ != aria2::DOWNLOAD_ACTIVE && status_ != aria2::DOWNLOAD_WAITING) return -1;
    return pauseDownload(QAria2::qaria2()->session(), gid_, force);
}

int QAria2Downloader::start()
{
    if(!gid_) return -1;
    if(status_ != aria2::DOWNLOAD_PAUSED) return -1;
    return unpauseDownload(QAria2::qaria2()->session(), gid_);
}

int QAria2Downloader::stop(bool force)
{
    if(!gid_) return -1;
    return removeDownload(QAria2::qaria2()->session(), gid_, force);
}
