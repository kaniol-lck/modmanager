#include "optifinemoditemwidget.h"
#include "ui_optifinemoditemwidget.h"

#include "optifine/optifinemod.h"
#include "download/downloadmanager.h"
#include "local/localmodpath.h"
#include "util/funcutil.h"
#include "config.h"

OptifineModItemWidget::OptifineModItemWidget(QWidget *parent, OptifineMod *mod) :
    QWidget(parent),
    ui(new Ui::OptifineModItemWidget),
    mod_(mod)
{
    ui->setupUi(this);
    QPixmap pixmap(":/image/optifine.png");
    ui->modIcon->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->downloadSpeedText->setVisible(false);
    ui->downloadProgress->setVisible(false);

    ui->displayNameText->setText(mod_->modInfo().name());
    ui->gameVersion->setText("Minecraft " + mod_->modInfo().gameVersion());
    if(mod_->modInfo().downloadUrl().isEmpty()){
        ui->downloadButton->setDisabled(true);
        connect(mod_, &OptifineMod::downloadUrlReady, this, [=]{
            if(ui->downloadButton->text() != tr("Downloaded")) ui->downloadButton->setEnabled(true);
        });
    }
}

OptifineModItemWidget::~OptifineModItemWidget()
{
    delete ui;
}

void OptifineModItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    DownloadFileInfo info(mod_->modInfo());
    if(downloadPath_)
        info.setPath(downloadPath_->info().path());
    else
        info.setPath(Config().getDownloadPath());

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

void OptifineModItemWidget::setDownloadPath(LocalModPath *newDownloadPath)
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

OptifineMod *OptifineModItemWidget::mod() const
{
    return mod_;
}

