#ifndef QARIA2DOWNLOADER_H
#define QARIA2DOWNLOADER_H

#include "abstractdownloader.h"

#include <aria2/aria2.h>

class QAria2Downloader : public AbstractDownloader
{
    Q_OBJECT
    friend class QAria2;
public:
    explicit QAria2Downloader(aria2::A2Gid fid = {});
    explicit QAria2Downloader(const DownloadFileInfo &info);
    // 析构时必须自己释放句柄：句柄归本对象所有（见 update()），
    // 只靠 QAria2 退出时统一释放的话，任何提前销毁的下载器都会漏一个句柄。
    ~QAria2Downloader();

    void deleteDownloadHandle();
    void update();

    // 让这个下载器以"失败"收场。用于任务根本没能提交给 aria2 的情况
    // （addUri 返回负值、或连下载地址都没拿到）——
    // 这类任务不会再有 aria2 事件回来，不主动补一次失败通知，
    // 等它的人（更新流程的完成计数）就会永远等下去。
    // 内部走队列调用，保证调用方 connect 之后才收到信号。
    void reportFailure();

    aria2::DownloadStatus status() const;

    bool isStarted() const override;
    bool isPaused() const override;
    bool isStopped() const override;
    // 完成 / 失败 / 被移除：不会再有新的进度，可以停止轮询
    bool isTerminal() const;

    aria2::DownloadHandle *handle() const;

public slots:
    int pause(bool force = false) override;
    int start() override;
    int stop(bool force = false) override;

signals:
    void handleUpdated();
//    void statusChanged(int);

private:
    void setGid(aria2::A2Gid newGid);
    void setEvent(const aria2::DownloadEvent &event);
    // 只报告一次终态结果（finished / downloadFailed）
    void reportResult(bool success);

    aria2::A2Gid gid_;
    aria2::DownloadHandle *handle_ = nullptr;
    aria2::DownloadStatus status_ = aria2::DOWNLOAD_WAITING;
    bool resultReported_ = false;
    // 连续几次取不到句柄（见 update()）
    int nullHandleTicks_ = 0;
};

#endif // QARIA2DOWNLOADER_H
