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
    ui->displayNameText->setProperty("class", "Title");
    ui->modIcon->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->downloadSpeedText->setVisible(false);
    ui->downloadProgress->setVisible(false);
    ui->downloadButton->setEnabled(false);

    onDownloadPathChanged();
    connect(browser_->downloadPathSelectMenu(), &DownloadPathSelectMenu::DownloadPathChanged, this, &OptifineModItemWidget::onDownloadPathChanged);

    ui->displayNameText->setText(mod_->modInfo().name());
    ui->gameVersion->setText("Minecraft " + mod_->modInfo().gameVersion());
    // 先接好信号再发起解析：acquireDownloadUrl() 在"已有直链"（BMCLAPI 走的同步路径）
    // 和"连文件名都没有"两种情况下是**同步** emit 的，连晚了这条通知会直接丢掉。
    connect(mod_, &OptifineMod::downloadUrlReady, this, &OptifineModItemWidget::onDownloadPathChanged);
    connect(mod_, &OptifineMod::downloadUrlFailed, this, &OptifineModItemWidget::onDownloadUrlFailed);
    mod->acquireDownloadUrl();
}

OptifineModItemWidget::~OptifineModItemWidget()
{
    delete ui;
}

void OptifineModItemWidget::on_downloadButton_clicked()
{
    // 兜底：没有 URL 时 downloadNewMod 会拿着空地址去建任务（必然失败），不如不点
    if(mod_->modInfo().downloadUrl().isEmpty()) return;
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
    // 失败也必须把按钮还原，否则会永久停在 "Downloading" 且不可点（见 curseforgefileitemwidget）
    connect(downloader, &AbstractDownloader::downloadFailed, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->clear();
        onDownloadPathChanged();
    });
}

void OptifineModItemWidget::onDownloadPathChanged()
{
    // 下载地址还没拿到时不能点：downloadNewMod 需要一个真实 URL。
    // 原来这里直接 return，按钮就停在构造函数里设的"禁用"状态、且没有任何提示，
    // 而配套的 acquireDownloadUrl() 又把失败路径吞掉了 —— 于是按钮永远不亮。
    if(mod_->modInfo().downloadUrl().isEmpty()){
        ui->downloadButton->setEnabled(false);
        ui->downloadButton->setText(tr("Download"));
        return;
    }
    ui->downloadButton->setToolTip({});
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

void OptifineModItemWidget::onDownloadUrlFailed()
{
    // 取不到直链时给个明确状态，别让用户对着一个没反应的灰按钮猜
    ui->downloadButton->setEnabled(false);
    ui->downloadButton->setText(tr("Download"));
    ui->downloadButton->setToolTip(tr("Failed to acquire the download url."));
}

