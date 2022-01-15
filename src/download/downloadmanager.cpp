#include "downloadmanager.h"

#include "qaria2.h"
#include "qaria2downloader.h"

#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgefile.h"
#include "curseforge/curseforgeapi.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    qaria2_(QAria2::qaria2())
{}

DownloadManager *DownloadManager::manager()
{
    static DownloadManager manager;
    return &manager;
}

QAria2Downloader *DownloadManager::download(const DownloadFileInfo &info)
{
    auto downloader = new QAria2Downloader(info);
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
