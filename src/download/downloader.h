#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QVector>
#include <QDir>

class QNetworkAccessManager;

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);

    bool download(const QUrl &url, const QDir &path = QDir(), const QString &filename = "");

signals:
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadInfoReady();

private slots:
    void threadFinished(int index);
    void updateProgress(int index, qint64 threadBytesReceived);
    void startDownload();

private:
    const int THREAD_COUNT;

    int finishedThreadCount_ = 0;
    QUrl url_;
    QFile file_;
    qint64 size_;
    QVector<qint64> bytesReceived_;
    QVector<qint64> bytesTotal_;

    void handleRedirect();

};

#endif // DOWNLOADER_H
