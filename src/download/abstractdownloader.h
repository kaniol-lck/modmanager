#ifndef ABSTRACTDOWNLOADER_H
#define ABSTRACTDOWNLOADER_H

#include "downloadfileinfo.h"

#include <QObject>
#include <QUrl>

class AbstractDownloader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDownloader(QObject *parent = nullptr);
    explicit AbstractDownloader(QObject *parent, const DownloadFileInfo &info);
    virtual ~AbstractDownloader() = 0;

    void handleRedirect();
    static QUrl handleRedirect(const QUrl &url);

    const DownloadFileInfo &info() const;
    void setInfo(const DownloadFileInfo &newInfo);
    void setIcon(const QPixmap &newIcon);

signals:
    void infoChanged();

    void redirected(QUrl url);
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadSpeed(qint64 download, qint64 upload = 0);

protected:
    DownloadFileInfo info_;
};

#endif // ABSTRACTDOWNLOADER_H
