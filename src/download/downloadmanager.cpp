#include "downloadmanager.h"

#include "qaria2.h"
#include "qaria2downloader.h"

#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgefile.h"
#include "curseforge/curseforgeapi.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    qaria2_(QAria2::qaria2()),
    model_(new DownloadManagerModel(this))
{}

DownloadManager *DownloadManager::manager()
{
    static DownloadManager manager;
    return &manager;
}

QAria2Downloader *DownloadManager::download(const DownloadFileInfo &info)
{
    auto downloader = new QAria2Downloader(info);
    model_->beginInsertRows(QModelIndex(), downloaders_.size(), downloaders_.size());
    downloaders_ << downloader;
    model_->endInsertRows();
    qDebug() << "new download job" << info.displayName();
    emit downloaderAdded(info, downloader);
    //handle redirect
    downloader->handleRedirect();
    connect(downloader, &AbstractDownloader::redirected, this, [=]{
        qaria2_->download(downloader);
    });
    return downloader;
}

QAria2Downloader *DownloadManager::download(CurseforgeMod *mod, CurseforgeFile *file, const QString &name, const QString &path)
{
    DownloadFileInfo info;
    info.setTitle(name);
    auto downloader = new QAria2Downloader(info);
    model_->beginInsertRows(QModelIndex(), downloaders_.size(), downloaders_.size());
    downloaders_ << downloader;
    model_->endInsertRows();
    emit downloaderAdded(info, downloader);
    auto setIcon = [=]{
        downloader->setIcon(mod->modInfo().icon().scaled(96, 96, Qt::KeepAspectRatio));
    };
    if(mod->modInfo().icon().isNull())
        connect(mod, &CurseforgeMod::iconReady, this, setIcon);
    else
        setIcon();
    auto setFileInfo = [=]{
        bool hasIcon = !downloader->info().icon().isNull();
        QPixmap icon;
        if(hasIcon) icon = downloader->info().icon();
        DownloadFileInfo info(file->info());
        info.setTitle(name);
        info.setPath(path);
        if(hasIcon) info.setIcon(icon);
        downloader->setInfo(info);
        //handle redirect
        downloader->handleRedirect();
    };
    if(file->info().url().isEmpty())
        connect(file, &CurseforgeFile::infoReady, this, setFileInfo);
    else
        setFileInfo();
    connect(downloader, &AbstractDownloader::redirected, this, [=]{
        qaria2_->download(downloader);
    });
    return downloader;
}

QAria2 *DownloadManager::qaria2() const
{
    return qaria2_;
}

DownloadManagerModel *DownloadManager::model() const
{
    return model_;
}

const QList<AbstractDownloader *> &DownloadManager::downloaders() const
{
    return downloaders_;
}

DownloadManagerModel::DownloadManagerModel(DownloadManager *manager) :
    QAbstractListModel(manager),
    manager_(manager)
{}

int DownloadManagerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return manager_->downloaders().size();
}

QVariant DownloadManagerModel::data(const QModelIndex &index, int role) const
{
    //    qDebug() << index << role;
        if (!index.isValid() || index.row() >= rowCount())
            return QVariant();
        auto downloader = manager_->downloaders().at(index.row());
        switch (role) {
        case Qt::UserRole + 1:
            return QVariant::fromValue(downloader);
            break;
        case Qt::SizeHintRole:
            return QSize(0, itemHeight_);
            break;
        }
        return QVariant();
}

void DownloadManagerModel::setItemHeight(int newItemHeight)
{
    itemHeight_ = newItemHeight;
}
