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

    static ModDownloader *addModDownload(const DownloadFileInfo &info, const QString &path = "");
    static ModDownloader *addModUpdate(const DownloadFileInfo &info, const QString &path, std::function<void()> finishCallback);

    const QList<ModDownloader *> &downloadList() const;

signals:
    void downloaderAdded(ModDownloader *downloader);

    void downloadSpeed(qint64 bytesPerSec);

public slots:
    void tryDownload();

private:
    void addDownloader(ModDownloader *downloader);

    QTimer speedTimer_;

    const int DOWNLOAD_COUNT;
    QList<ModDownloader*> downloadList_;
    QList<qint64> speedList_;

};

#endif // DOWNLOADMANAGER_H
