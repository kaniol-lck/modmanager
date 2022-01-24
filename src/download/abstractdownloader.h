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

    virtual bool isStarted() const = 0;
    virtual bool isPaused() const = 0;

    const DownloadFileInfo &info() const;

public slots:
    virtual void pause() = 0;
    virtual void start() = 0;

    void handleRedirect();
    static QUrl handleRedirect(const QUrl &url);

    void setInfo(const DownloadFileInfo &newInfo);
    void setIcon(const QPixmap &newIcon);

signals:
    void infoChanged();
    void statusChanged(int);

    void redirected(QUrl url);
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadSpeed(qint64 download, qint64 upload = 0);

protected:
    DownloadFileInfo info_;
};

#endif // ABSTRACTDOWNLOADER_H
