#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>

#include "downloadfileinfo.h"

class CurseforgeFile;
class QAria2;
class QAria2Downloader;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    static DownloadManager *manager();

    QAria2Downloader *download(const DownloadFileInfo &info);
    QAria2Downloader *download(CurseforgeMod *mod, CurseforgeFile *file, const QString &name, const QString &path);

    QAria2 *qaria2() const;

signals:
    void downloaderAdded(DownloadFileInfo info, QAria2Downloader *downloader);
private:
    QAria2 *qaria2_;
};

#endif // DOWNLOADMANAGER_H
