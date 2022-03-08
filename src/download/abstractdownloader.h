#ifndef ABSTRACTDOWNLOADER_H
#define ABSTRACTDOWNLOADER_H

#include "downloadfileinfo.h"
#include "speedrecorder.h"

#include <QUrl>

class AbstractDownloader : public SpeedRecorder
{
    Q_OBJECT
public:
    explicit AbstractDownloader(QObject *parent = nullptr, const DownloadFileInfo &info = {});
    virtual ~AbstractDownloader() = 0;

    virtual bool isStarted() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isStopped() const = 0;

    const DownloadFileInfo &info() const;

public slots:
    virtual int pause(bool force = false) = 0;
    virtual int start() = 0;
    virtual int stop(bool force = false) = 0;

    void handleRedirect();
    static QUrl handleRedirect(const QUrl &url);

    void setInfo(const DownloadFileInfo &newInfo);
    void setIcon(const QPixmap &newIcon);

signals:
    void infoChanged();
    void statusChanged(int);

    void redirected(QUrl url);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:
    DownloadFileInfo info_;
};

#endif // ABSTRACTDOWNLOADER_H
