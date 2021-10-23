#include "curseforgefileitemwidget.h"
#include "ui_curseforgefileitemwidget.h"

#include <QDebug>
#include <QMenu>
#include <QClipboard>

#include "local/localmod.h"
#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"

CurseforgeFileItemWidget::CurseforgeFileItemWidget(QWidget *parent, CurseforgeMod *mod, const CurseforgeFileInfo &info, LocalMod *localMod) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileItemWidget),
    mod_(mod),
    localMod_(localMod),
    fileInfo_(info)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);

    updateLocalInfo();
    if(localMod_)
        connect(localMod_, &LocalMod::modCacheUpdated, this, &CurseforgeFileItemWidget::updateLocalInfo);

    //game version
    QString gameversionText;
    for(const auto &ver : fileInfo_.gameVersions())
        gameversionText.append(ver).append(" ");
    ui->gameVersionText->setText(gameversionText);

    //loader type
    QString loaderTypeText;
    for(const auto &loader : fileInfo_.loaderTypes())
        loaderTypeText.append(ModLoaderType::toString(loader)).append(" ");
    ui->loaderTypeText->setText(loaderTypeText);

    //size
    ui->downloadSpeedText->setText(numberConvert(fileInfo_.size(), "B"));
}

CurseforgeFileItemWidget::~CurseforgeFileItemWidget()
{
    delete ui;
}

void CurseforgeFileItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    ui->downloadProgress->setMaximum(fileInfo_.size());

    QAria2Downloader *downloader;
    DownloadFileInfo info(fileInfo_);
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    info.setIcon(pixelmap);
    if(localMod_)
        info.setIconBytes(localMod_->commonInfo()->iconBytes());
    else
        info.setIconBytes(mod_->modInfo().iconBytes());
    if(downloadPath_)
        downloader = downloadPath_->downloadNewMod(info);
    else if(localMod_)
        downloader = localMod_->downloadOldMod(info);
    else{
        info.setPath(Config().getDownloadPath());
        downloader = DownloadManager::manager()->download(info);
    }

    connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &AbstractDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(fileInfo_.size(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

void CurseforgeFileItemWidget::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;

    bool bl;
    if(downloadPath_)
        bl = hasFile(downloadPath_, fileInfo_);
    else
        bl = hasFile(Config().getDownloadPath(), fileInfo_.fileName());

    if(bl){
        ui->downloadButton->setEnabled(false);
        ui->downloadButton->setText(tr("Downloaded"));
    } else{
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setText(tr("Download"));
    }
}

void CurseforgeFileItemWidget::updateLocalInfo()
{
    auto name = fileInfo_.displayName();
    if(localMod_ && localMod_->curseforgeUpdate().currentFileInfo() && localMod_->curseforgeUpdate().currentFileInfo()->id() == fileInfo_.id())
        name.prepend("<font color=\"#56a\">" + tr("[Current]") + "</font> ");
    ui->displayNameText->setText(name);
    //refresh downloaded infos
    setDownloadPath(downloadPath_);
}

void CurseforgeFileItemWidget::on_CurseforgeFileItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    connect(menu->addAction(tr("Copy download link")), &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(fileInfo_.url().toString());
    });
    if(localMod_ && !(localMod_->curseforgeUpdate().currentFileInfo() && localMod_->curseforgeUpdate().currentFileInfo()->id() == fileInfo_.id()))
        connect(menu->addAction(tr("Set as current")), &QAction::triggered, this, [=]{
            localMod_->setCurrentCurseforgeFileInfo(fileInfo_);
        });
    if(!menu->isEmpty())
        menu->exec(mapToGlobal(pos));
}
