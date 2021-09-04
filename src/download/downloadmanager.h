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

    static ModDownloader *addModDownload(std::shared_ptr<DownloadFileInfo> info, QString path = "");
    static ModDownloader *addModupdate(std::shared_ptr<DownloadFileInfo> info, QString path, std::function<void()> finishCallback);

    const QList<ModDownloader *> &getDownloadList() const;

signals:
    void downloaderAdded(ModDownloader *downloader);

    void downloadSpeed(qint64 bytesPerSec);

public slots:
    void tryDownload();

private:
    void addDownloader(ModDownloader *downloader);

    QTimer speedTimer;

    const int DOWNLOAD_COUNT;
    QList<ModDownloader*> downloadList;
    QList<qint64> speedList;

};

#endif // DOWNLOADMANAGER_H
