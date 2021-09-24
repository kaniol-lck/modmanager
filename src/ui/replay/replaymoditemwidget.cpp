#include "replaymoditemwidget.h"
#include "ui_replaymoditemwidget.h"

#include "local/localmodpath.h"
#include "replay/replaymod.h"
#include "util/funcutil.h"
#include "config.h"

ReplayModItemWidget::ReplayModItemWidget(QWidget *parent, ReplayMod *mod) :
    QWidget(parent),
    ui(new Ui::ReplayModItemWidget),
    mod_(mod)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));

    ui->displayNameText->setText(mod_->modInfo().name());
    ui->gameVersion->setText(mod_->modInfo().gameVersionString());
}

ReplayModItemWidget::~ReplayModItemWidget()
{
    delete ui;
}

ReplayMod *ReplayModItemWidget::mod() const
{
    return mod_;
}

void ReplayModItemWidget::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;

    bool bl;
    if(downloadPath_)
        bl = hasFile(downloadPath_->info().path(), mod_->modInfo().fileName()); //TODO
    else
        bl = hasFile(Config().getDownloadPath(), mod_->modInfo().fileName()); //TODO

    if(bl){
        ui->downloadButton->setEnabled(false);
        ui->downloadButton->setText(tr("Downloaded"));
    } else{
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setText(tr("Download"));
    }
}

void ReplayModItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    DownloadFileInfo info(mod_->modInfo());
    if(downloadPath_)
        info.setPath(downloadPath_->info().path());
    else
        info.setPath(Config().getDownloadPath());
//    info.setIconBytes();

    auto downloader = DownloadManager::addModDownload(info);
    connect(downloader, &Downloader::sizeUpdated, this, [=](qint64 size){
        ui->downloadProgress->setMaximum(size);
    });
    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

