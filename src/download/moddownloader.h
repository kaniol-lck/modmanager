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

    void downloadMod(std::shared_ptr<DownloadFileInfo> info, QString path);
    void updateMod(std::shared_ptr<DownloadFileInfo> info, QString path);
    void startDownload();

    enum DownloadStatus { Idol, Queue, Downloading, Paused, Finished };

    enum DownloadType { Download, Update, Custom };

    const std::shared_ptr<DownloadFileInfo> &getFileInfo() const;
    DownloadStatus getStatus() const;
    DownloadType getType() const;
    const QString &getReadySize() const;
    const QString &getFilePath() const;

    void setStatus(DownloadStatus newStatus);

signals:
    void statusChanged();

    void downloadSpeed(qint64 bytesPerSec);

private:
    QTimer speedTimer;
    qint64 currentDownloadBytes = 0;
    qint64 lastDownloadBytes = 0;
    QList<qint64> downloadBytes;
    std::shared_ptr<DownloadFileInfo> fileInfo;
    DownloadStatus status = DownloadStatus::Idol;
    DownloadType type;
    QString readySize;
    QString filePath;
};

#endif // MODDOWNLOADER_H
