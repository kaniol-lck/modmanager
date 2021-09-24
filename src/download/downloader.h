#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QVector>

class QNetworkAccessManager;

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);

    void addDownload(const QUrl &url, const QString &path = "", const QString &fileName = "");
    bool startDownload();

    QString filePath() const;

    const QUrl &url() const;
    const QFile &file() const;
    qint64 size() const;

    bool readyDownload() const;

signals:
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadInfoReady();
    void waitForRedirect();

    void sizeUpdated(qint64 size);

private slots:
    void threadFinished(int index);
    void updateProgress(int index, qint64 threadBytesReceived);

private:
    const int THREAD_COUNT;

    int finishedThreadCount_ = 0;
    QUrl url_;
    QFile file_;
    qint64 size_;
    QVector<qint64> bytesReceived_;
    QVector<qint64> bytesTotal_;

    bool readyDownload_ = false;
    void handleRedirect();
};

#endif // DOWNLOADER_H
