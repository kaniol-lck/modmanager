#ifndef ABSTRACTDOWNLOADER_H
#define ABSTRACTDOWNLOADER_H

#include <QObject>
#include <QUrl>

class AbstractDownloader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDownloader(QObject *parent = nullptr);
    explicit AbstractDownloader(QObject *parent, const QUrl &url, const QString &path);
//    virtual ~AbstractDownloader() = 0;
    void handleRedirect();
    static QUrl handleRedirect(const QUrl &url);
    const QUrl &url() const;
    const QString &path() const;
signals:
    void redirected(QUrl url);
    void finished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadSpeed(qint64 download, qint64 upload = 0);
protected:
    QUrl url_;
    QString path_;
};

#endif // ABSTRACTDOWNLOADER_H
