#ifndef MODDOWNLOADER_H
#define MODDOWNLOADER_H

#include <QTimer>
#include <memory>

#include "downloader.h"
#include "downloadfileinfo.h"

class ModDownloader : public Downloader
{
    Q_OBJECT
public:
    explicit ModDownloader(QObject *parent = nullptr);

    void downloadMod(const DownloadFileInfo &info);
    void updateMod(const DownloadFileInfo &info);
    void startDownload();

    enum DownloadStatus { Idol, Queue, Downloading, Paused, Finished };

    enum DownloadType { Download, Update, Custom };

    const DownloadFileInfo &fileInfo() const;
    DownloadStatus status() const;
    DownloadType type() const;
    const QString &readySize() const;

    void setStatus(DownloadStatus newStatus);

signals:
    void statusChanged();

    void downloadSpeed(qint64 bytesPerSec);

private:
    QTimer speedTimer_;
    qint64 currentDownloadBytes_ = 0;
    qint64 lastDownloadBytes_ = 0;
    DownloadFileInfo fileInfo_;
    DownloadStatus status_ = DownloadStatus::Idol;
    DownloadType type_;
    QString readySize_;
};

#endif // MODDOWNLOADER_H
