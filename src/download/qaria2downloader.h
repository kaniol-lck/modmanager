#ifndef QARIA2DOWNLOADER_H
#define QARIA2DOWNLOADER_H

#include "abstractdownloader.h"

#include <aria2/aria2.h>

class QAria2Downloader : public AbstractDownloader
{
    Q_OBJECT
    friend class QAria2;
public:
    explicit QAria2Downloader(aria2::A2Gid fid);
    explicit QAria2Downloader(const QUrl &url, const QString &path);
    ~QAria2Downloader() = default;

    void deleteDownloadHandle();
    void update();

    aria2::DownloadStatus status();

    void setGid(aria2::A2Gid newGid);

signals:
    void statusChanged(int);
private:
    void setEvent(const aria2::DownloadEvent &event);

    aria2::A2Gid gid_;
    aria2::DownloadHandle *handle_ = nullptr;
    mutable aria2::DownloadStatus status_ = aria2::DOWNLOAD_WAITING;
};

#endif // QARIA2DOWNLOADER_H
