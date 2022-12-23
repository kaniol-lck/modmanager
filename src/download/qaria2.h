#ifndef QARIA2_H
#define QARIA2_H

#include <QMap>
#include <QObject>
#include <QTimer>
#include <QUrl>
//#include <aria2/aria2.h>
#include<aria2.h>

class QAria2Downloader;

class QAria2 : public QObject
{
    Q_OBJECT
    static int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event, const aria2::A2Gid gid, void* userData);
public:
    explicit QAria2(QObject *parent = nullptr);
    ~QAria2();

    static QAria2 *qaria2();

    QAria2Downloader *downloadNoRedirect(const QUrl &url, const QString &path = QString());
    QAria2Downloader *download(const QUrl &url, const QString &path = QString());
    QAria2Downloader *download(QAria2Downloader *downloader);

    aria2::Session *session() const;
    const QList<QAria2Downloader *> &downloaders() const;

public slots:
    void updateOptions();
signals:
    void downloaderAdded(QAria2Downloader *downloader);
    void started();
    void finished();

    void downloadSpeed(qint64 download, qint64 upload);
    void globalDownloadStat(int inactive, int active, int gdl, int gup);
    void downloadStatPerItem(uint id, int completed, int total,int perDl, int perUp);
private:
    //we only use one session
    aria2::Session* session_;
    aria2::SessionConfig config_;
    aria2::KeyVals options;
    QTimer timer_;
    QList<QAria2Downloader*> downloaders_;
};

#endif // QARIA2_H
