#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <functional>

#include "downloadfileinfo.h"
#include "moddownloader.h"

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);

    static DownloadManager *manager();

    static ModDownloader *addModDownload(const DownloadFileInfo &info);
    static ModDownloader *addModUpdate(const DownloadFileInfo &info, std::function<void()> finishCallback);

    const QList<Downloader *> &downloadList() const;


signals:
    void downloaderAdded(Downloader *downloader);

    void downloadSpeed(qint64 bytesPerSec);

public slots:
    void tryDownload();
    void saveToConfig();

private:
    void addDownloader(Downloader *downloader);

    QTimer speedTimer_;

    const int DOWNLOAD_COUNT;
    QList<Downloader*> downloadList_;
    QList<qint64> speedList_;
};

#endif // DOWNLOADMANAGER_H
