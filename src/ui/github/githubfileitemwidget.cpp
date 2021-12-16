#include "githubfileitemwidget.h"
#include "ui_githubfileitemwidget.h"

#include "local/localmodpath.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"

GitHubFileItemWidget::GitHubFileItemWidget(QWidget *parent, const GitHubFileInfo &info) :
    QWidget(parent),
    ui(new Ui::GitHubFileItemWidget),
    info_(info)
{
    ui->setupUi(this);
    ui->displayNameText->setText(info_.name());
    ui->downloadProgress->setVisible(false);
    //size
    ui->downloadSpeedText->setText(sizeConvert(info_.size()));
}

GitHubFileItemWidget::~GitHubFileItemWidget()
{
    delete ui;
}

void GitHubFileItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    ui->downloadProgress->setMaximum(info_.size());

    QAria2Downloader *downloader;
    DownloadFileInfo info(info_);
//    if(localMod_)
//        info.setIconBytes(localMod_->commonInfo()->iconBytes());
//    else
//        info.setIconBytes(mod_->modInfo().iconBytes());
//    if(downloadPath_)
//        downloader = downloadPath_->downloadNewMod(info);
//    else if(localMod_)
//        downloader = localMod_->downloadOldMod(info);
//    else{
        info.setPath(Config().getDownloadPath());
        downloader = DownloadManager::manager()->download(info);
//    }

    connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &AbstractDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(speedConvert(bytesPerSec));
    });
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(sizeConvert(info_.size()));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

