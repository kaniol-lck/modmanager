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

    bool download(const QUrl &url, const QString &filename = "", int size = 0);

signals:
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void threadFinished(int index);
    void updateProgress(int index, qint64 threadBytesReceived, qint64 threadBytesTotal);

private:
    static constexpr int THREAD_COUNT = 8;

    int finishedThreadCount = 0;
    QNetworkAccessManager *accessManager;
    QUrl downloadUrl;
    QFile downloadFile;
    qint64 downloadSize;
    QVector<qint64> bytesReceived;
    QVector<qint64> bytesTotal;

};

#endif // DOWNLOADER_H
