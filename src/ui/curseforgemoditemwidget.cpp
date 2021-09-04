#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include <QDebug>
#include <algorithm>

#include "curseforge/curseforgemod.h"
#include "download/downloadmanager.h"
#include "download/downloader.h"
#include "util/funcutil.h"

CurseforgeModItemWidget::CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    curseforgeMod(mod),
    defaultFileInfo(defaultDownload)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    connect(curseforgeMod, &CurseforgeMod::iconReady, this, &CurseforgeModItemWidget::updateIcon);

    ui->modName->setText(mod->getModInfo().getName());
    ui->modSummary->setText(mod->getModInfo().getSummary());
    ui->modAuthors->setText(mod->getModInfo().getAuthors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    if(defaultFileInfo.has_value()){
        ui->downloadButton->setToolTip(defaultDownload.value().getDisplayName());
        ui->downloadSpeedText->setText(numberConvert(defaultDownload.value().getSize(), "B") + "\n"
                                   + numberConvert(mod->getModInfo().getDownloadCount(), "", 3, 1000) + tr(" Downloads"));
    }
    else
        ui->downloadButton->setEnabled(false);
}

CurseforgeModItemWidget::~CurseforgeModItemWidget()
{
    delete ui;
}

void CurseforgeModItemWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(curseforgeMod->getModInfo().getIconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}

void CurseforgeModItemWidget::on_downloadButton_clicked()
{
    if(!defaultFileInfo.has_value()) return;

    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    //TODO: path
    auto downloader = DownloadManager::addModDownload(std::make_shared<CurseforgeFileInfo>(defaultFileInfo.value()));

    ui->downloadProgress->setMaximum(defaultFileInfo.value().getSize());

    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(defaultFileInfo.value().getSize(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}
