#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "abstractdownloader.h"

#include <QFile>
#include <QUrl>
#include <QVector>
#include <QTimer>

class DownloaderThread;

class Downloader : public AbstractDownloader
{
    Q_OBJECT
public:
    enum DownloadStatus { Idol, Perparing, Queue, Downloading, Paused, Finished, Error };
    enum DownloadType { Download, Update, Custom };

    explicit Downloader(QObject *parent = nullptr);
    explicit Downloader(QObject *parent, const QVariant &variant);
    ~Downloader() = default;
    QVariant toVariant() const;

    void addDownload(const QUrl &url, const QString &path = "", const QString &fileName = "");
    bool startDownload();
    void pauseDownload();
    bool resumeDownload();

    const QUrl &url() const;
    const QFile &file() const;
    qint64 size() const;

    DownloadStatus status() const;
    void setStatus(DownloadStatus newStatus);

signals:
    void statusChanged(DownloadStatus status);
    void finished();
    void sizeUpdated(qint64 size);

    void waitForRedirect();
private slots:
    void threadFinished(int index);
    void updateProgress(int index, qint64 threadBytesReceived);

protected:
    DownloadType type_;

private:
    int threadCount_;
    int finishedThreadCount_ = 0;
    QUrl url_;
    QFile file_;
    qint64 size_;
    QVector<DownloaderThread*> threads_;
    QVector<qint64> bytesReceived_;
    QVector<qint64> bytesTotal_;

    QTimer speedTimer_;
    qint64 currentDownloadBytes_ = 0;
    qint64 lastDownloadBytes_ = 0;
    DownloadStatus status_ = DownloadStatus::Idol;

    void handleRedirect();
};

#endif // DOWNLOADER_H
