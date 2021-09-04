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

    ModDownloader *addModDownload(std::shared_ptr<DownloadFileInfo> info, QString path = "");
    ModDownloader *addModupdate(std::shared_ptr<DownloadFileInfo> info, QString path, std::function<void()> finishCallback);

    const QList<ModDownloader *> &getDownloadList() const;

signals:
    void downloaderAdded(ModDownloader *downloader);

public slots:
    void tryDownload();

signals:

private:
    void addDownloader(ModDownloader *downloader);

    const int DOWNLOAD_COUNT;
    QList<ModDownloader*> downloadList;

};

#endif // DOWNLOADMANAGER_H
