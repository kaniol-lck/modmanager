#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include <QDebug>
#include <algorithm>

#include "curseforge/curseforgemod.h"
#include "util/downloader.h"
#include "util/funcutil.h"

constexpr int TIMER_PER_SEC = 4;

CurseforgeModItemWidget::CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    curseforgeMod(mod),
    curseforgeFileInfo(defaultDownload)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, &CurseforgeModItemWidget::updateThumbnail);

    ui->modName->setText(mod->getModInfo().getName());
    ui->modSummary->setText(mod->getModInfo().getSummary());
    ui->modAuthors->setText(mod->getModInfo().getAuthors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    if(curseforgeFileInfo.has_value()){
        ui->downloadButton->setToolTip(defaultDownload.value().getDisplayName());
        ui->downloadSpeedText->setText(numberConvert(defaultDownload.value().getFileLength(), "B") + "\n"
                                   + numberConvert(mod->getModInfo().getDownloadCount(), "", 3, 1000) + tr(" Downloads"));
    }
    else
        ui->downloadButton->setEnabled(false);

    //set timer
    speedTimer.setInterval(1000 / TIMER_PER_SEC);
    connect(&speedTimer, SIGNAL(timeout()), SLOT(updateDownlaodSpeed()));
}

CurseforgeModItemWidget::~CurseforgeModItemWidget()
{
    delete ui;
}

void CurseforgeModItemWidget::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(curseforgeMod->getModInfo().getThumbnailBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}

void CurseforgeModItemWidget::updateDownlaodSpeed()
{
    auto currentDownloadBytes = ui->downloadProgress->value();
    auto bytes = currentDownloadBytes - lastDownloadBytes;
    lastDownloadBytes = currentDownloadBytes;

    downloadBytes << bytes;
    if(downloadBytes.size() > 4) downloadBytes.pop_front();

    auto aver = std::accumulate(downloadBytes.cbegin(), downloadBytes.cend(), 0) / downloadBytes.size();

    ui->downloadSpeedText->setText(numberConvert(aver * TIMER_PER_SEC , "B/s"));

}

void CurseforgeModItemWidget::on_downloadButton_clicked()
{
    if(!curseforgeFileInfo.has_value()) return;

    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    auto downloader = new Downloader(this);

    //download latest file by defsult
    //TODO: select file to download
    downloader->download(curseforgeFileInfo.value().getDownloadUrl(), curseforgeFileInfo.value().getFileName());
    ui->downloadProgress->setMaximum(curseforgeFileInfo.value().getFileLength());

    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(curseforgeFileInfo.value().getFileLength(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
        speedTimer.stop();
        lastDownloadBytes = 0;
        downloadBytes.clear();
    });

    speedTimer.start();
}
