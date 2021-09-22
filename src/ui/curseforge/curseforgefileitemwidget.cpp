#include "curseforgefileitemwidget.h"
#include "ui_curseforgefileitemwidget.h"

#include <QDebug>

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
    ui->displayNameText->setText(fileInfo_.displayName());
    ui->downloadProgress->setVisible(false);

    //file name and link
    QString linkText = fileInfo_.fileName();
    linkText = "<a href=%1>" + linkText + "</a>";
    ui->fileNameText->setText(linkText.arg(fileInfo_.url().toString()));

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

    DownloadFileInfo info(fileInfo_);
    if(localMod_)
        info.setIconBytes(localMod_->commonInfo()->iconBytes());
    else
        info.setIconBytes(mod_->modInfo().iconBytes());
    if(downloadPath_)
        info.setPath(downloadPath_->info().path());
    else
        info.setPath(Config().getDownloadPath());

    auto downloader = DownloadManager::addModDownload(info);
    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(fileInfo_.size(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
        if(localMod_){
            auto file = new LocalModFile(this, downloader->filePath());
            if(file->loaderType() == localMod_->modFile()->loaderType()){
                file->addOld();
                localMod_->addOldFile(file);
            } else
                file->deleteLater();
        }
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
