#ifndef DOWNLOADERTHREAD_H
#define DOWNLOADERTHREAD_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class QFile;

class DownloaderThread : public QObject
{
    Q_OBJECT

signals:
    void threadFinished(int index);
    void threadDownloadProgress(int index, qint64 bytesReceived);
    void threadErrorOccurred(int index, QNetworkReply::NetworkError code);

public:
    explicit DownloaderThread(QObject *parent, int index, const QUrl &url, QFile *file, qint64 startPos, qint64 endPos);

    void start();
    void stop();

private slots:
    void writeFile();

private:
    QNetworkAccessManager accessManager_;
    QNetworkReply *reply_;
    int index_;
    QUrl url_;
    QFile *file_;
    qint64 threadStartPos_;
    qint64 threadEndPos_;
    qint64 readySize_ = 0;
    bool paused_ = false;
};

#endif // DOWNLOADERTHREAD_H
