#include "optifinemodbrowser.h"
#include "optifinemoditemwidget.h"
#include "ui_optifinemoditemwidget.h"

#include "optifine/optifinemod.h"
#include "download/downloadmanager.h"
#include "local/localmodpath.h"
#include "util/funcutil.h"
#include "config.hpp"
#include "ui/downloadpathselectmenu.h"

OptifineModItemWidget::OptifineModItemWidget(OptifineModBrowser *parent, OptifineMod *mod) :
    QWidget(parent),
    ui(new Ui::OptifineModItemWidget),
    browser_(parent),
    mod_(mod)
{
    ui->setupUi(this);
    QPixmap pixmap(":/image/optifine.png");
    ui->modIcon->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->downloadSpeedText->setVisible(false);
    ui->downloadProgress->setVisible(false);
    ui->downloadButton->setEnabled(false);

    onDownloadPathChanged();
    connect(browser_->downloadPathSelectMenu(), &DownloadPathSelectMenu::DownloadPathChanged, this, &OptifineModItemWidget::onDownloadPathChanged);

    ui->displayNameText->setText(mod_->modInfo().name());
    ui->gameVersion->setText("Minecraft " + mod_->modInfo().gameVersion());
    mod->acquireDownloadUrl();
    connect(mod_, &OptifineMod::downloadUrlReady, this, &OptifineModItemWidget::onDownloadPathChanged);
}

OptifineModItemWidget::~OptifineModItemWidget()
{
    delete ui;
}

void OptifineModItemWidget::on_downloadButton_clicked()
{
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    QAria2Downloader *downloader;
    DownloadFileInfo info(mod_->modInfo());
    if(auto downloadPath = browser_->downloadPath())
        downloader = downloadPath->downloadNewMod(info);
    else{
        info.setPath(Config().getDownloadPath());
        downloader = DownloadManager::manager()->download(info);
    }
    connect(downloader, &AbstractDownloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){
        ui->downloadProgress->setValue(bytesReceived);
        ui->downloadProgress->setMaximum(bytesTotal);
    });
    connect(downloader, &AbstractDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(speedConvert(bytesPerSec));
    });
    connect(downloader, &AbstractDownloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

void OptifineModItemWidget::onDownloadPathChanged()
{
    if(mod_->modInfo().downloadUrl().isEmpty()) return;
    bool bl = false;
    if(auto downloadPath = browser_->downloadPath())
        bl = hasFile(downloadPath->info().path(), mod_->modInfo().fileName()); //TODO
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

