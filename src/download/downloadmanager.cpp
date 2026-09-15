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
    qDebug() << "new download job" << info.displayName() << info.filePath();
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
        if(info.url().isEmpty()){
            // 连下载地址都没拿到（文件信息接口失败 / 文件已被删除）。
            // 这时候再去 handleRedirect + addUri 只会拿空地址去撞，直接报失败更干脆，
            // 也让"下载列表里挂着一条永远不会动的任务"不至于发生。
            downloader->reportFailure();
        } else
            //handle redirect
            downloader->handleRedirect();
        // 信息补齐之后才对外通告，监听者拿到的才是一条完整（含 url / path / 文件名）的下载任务
        emit downloaderAdded(info, downloader);
    };
    if(file->info().url().isEmpty()){
        connect(file, &CurseforgeFile::infoReady, this, setFileInfo);
        // infoReady 在"取信息失败"时同样会发一次，之后就再也不会发。
        // 如果调用到这里的这一刻早先那次取信息已经失败过，上面这行 connect 就永远等不到信号：
        // downloader 的 url 一直为空 → 既不 addUri 也不报错 → 这个下载静默卡死。
        // 这里补一次重取（正在取时 acquireFileInfo 会直接返回空，不会重复请求）。
        file->acquireFileInfo();
    } else
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
