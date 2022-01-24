#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QAbstractListModel>
#include <QObject>

#include "downloadfileinfo.h"

class AbstractDownloader;
class CurseforgeFile;
class QAria2;
class QAria2Downloader;
class DownloadManagerModel;
class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    static DownloadManager *manager();

    QAria2Downloader *download(const DownloadFileInfo &info);
    QAria2Downloader *download(CurseforgeMod *mod, CurseforgeFile *file, const QString &name, const QString &path);

    QAria2 *qaria2() const;

    DownloadManagerModel *model() const;

    const QList<AbstractDownloader *> &downloaders() const;

signals:
    void downloaderAdded(DownloadFileInfo info, QAria2Downloader *downloader);

private:
    QAria2 *qaria2_;
    QList<AbstractDownloader*> downloaders_;
    DownloadManagerModel *model_ = nullptr;
};

class DownloadManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend DownloadManager;
public:
    DownloadManagerModel(DownloadManager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    DownloadManager *manager_;
    int itemHeight_ = 100;
};
#endif // DOWNLOADMANAGER_H
