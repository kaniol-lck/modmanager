#ifndef ABSTRACTDOWNLOADER_H
#define ABSTRACTDOWNLOADER_H

#include "downloadfileinfo.h"

#include <QObject>
#include <QTimer>
#include <QUrl>

class AbstractDownloader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDownloader(QObject *parent = nullptr, const DownloadFileInfo &info = {});
    virtual ~AbstractDownloader() = 0;

    virtual bool isStarted() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isStopped() const = 0;

    const DownloadFileInfo &info() const;

    enum LineType { Download, Upload };
    struct PointData
    {
        qint64 x;
        qint64 y[2];
    };

    const QList<PointData> &dataCollection() const;
    void setDataCollection(const QList<PointData> &newDataCollection);
    static constexpr auto DATA_MAXSIZE = 60;

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
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadSpeed(qint64 download, qint64 upload = 0);

    void dataUpdated();

protected:
    DownloadFileInfo info_;

private:
    void addData(qint64 downSpeed, qint64 upSpeed);

    QTimer timer_;
    QList<PointData> dataCollection_;
    qint64 downSpeed_;
    qint64 upSpeed_;
};

#endif // ABSTRACTDOWNLOADER_H
