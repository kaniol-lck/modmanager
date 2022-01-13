#include "githubfileitemwidget.h"
#include "githubfilelistwidget.h"
#include "ui_githubfileitemwidget.h"

#include "local/localmodpath.h"
#include "util/funcutil.h"
#include "download/downloadmanager.h"
#include "ui/downloadpathselectmenu.h"

GitHubFileItemWidget::GitHubFileItemWidget(GitHubFileListWidget *parent, const GitHubFileInfo &info) :
    QWidget(parent),
    ui(new Ui::GitHubFileItemWidget),
    fileList_(parent),
    info_(info)
{
    ui->setupUi(this);
    ui->displayNameText->setProperty("class", "Title");
    ui->displayNameText->setText(info_.name());
    ui->downloadProgress->setVisible(false);
    //size
    ui->downloadSpeedText->setText(sizeConvert(info_.size()));

    onDownloadPathChanged();
    connect(fileList_->downloadPathSelectMenu(), &DownloadPathSelectMenu::DownloadPathChanged, this, &GitHubFileItemWidget::onDownloadPathChanged);
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
    if(auto downloadPath = fileList_->downloadPathSelectMenu()->downloadPath())
        downloader = downloadPath->downloadNewMod(info);
    else{
        info.setPath(Config().getDownloadPath());
        downloader = DownloadManager::manager()->download(info);
    }

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

void GitHubFileItemWidget::onDownloadPathChanged()
{
    bool bl = false;
    if(fileList_){
        if(auto downloadPath = fileList_->downloadPathSelectMenu()->downloadPath())
            bl = hasFile(downloadPath->info().path(), info_.name());
    }else
        bl = hasFile(Config().getDownloadPath(), info_.name());

    if(bl){
        ui->downloadButton->setEnabled(false);
        ui->downloadButton->setText(tr("Downloaded"));
    } else{
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setText(tr("Download"));
    }
}

