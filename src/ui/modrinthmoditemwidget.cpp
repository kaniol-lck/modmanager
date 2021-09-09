#include "modrinthmoditemwidget.h"
#include "ui_modrinthmoditemwidget.h"

#include <QMenu>

#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "util/funcutil.h"

ModrinthModItemWidget::ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod, const QString &path) :
    QWidget(parent),
    ui(new Ui::ModrinthModItemWidget),
    mod_(mod),
    downloadPath_(path)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);

    ui->modName->setText(mod->modInfo().name());
    ui->modSummary->setText(mod->modInfo().summary());
    ui->modAuthors->setText("by <b>" + mod->modInfo().author() + "</b>");
    ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));

//    mod->acquireFullInfo();
//    connect(mod, &ModrinthMod::fullInfoReady, mod, &ModrinthMod::acquireFileList);

    connect(mod, &ModrinthMod::iconReady, this, &ModrinthModItemWidget::updateIcon);
    connect(mod, &ModrinthMod::fileListReady, this, &ModrinthModItemWidget::updateFileList);

}

ModrinthModItemWidget::~ModrinthModItemWidget()
{
    delete ui;
}

void ModrinthModItemWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}

void ModrinthModItemWidget::updateFileList()
{
    auto menu = new QMenu(this);

    for(const auto &fileInfo : mod_->modInfo().featuredFileList()){
        auto name = fileInfo.displayName() + " ("+ numberConvert(fileInfo.size(), "B") + ")";
        connect(menu->addAction(name), &QAction::triggered, this, [=]{
//            downloadFile(fileInfo);l
        });
    }

    ui->downloadButton->setMenu(menu);
}

void ModrinthModItemWidget::downloadFile(const ModrinthFileInfo &fileInfo)
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    auto downloader = DownloadManager::addModDownload(std::make_shared<ModrinthFileInfo>(fileInfo), downloadPath_);

    ui->downloadProgress->setMaximum(fileInfo.size());

    connect(downloader, &Downloader::downloadProgress, this, [=](qint64 bytesReceived, qint64 /*bytesTotal*/){
        ui->downloadProgress->setValue(bytesReceived);
    });
    connect(downloader, &ModDownloader::downloadSpeed, this, [=](qint64 bytesPerSec){
        ui->downloadSpeedText->setText(numberConvert(bytesPerSec, "B/s"));
    });
    connect(downloader, &Downloader::finished, this, [=]{
        ui->downloadProgress->setVisible(false);
        ui->downloadSpeedText->setText(numberConvert(fileInfo.size(), "B"));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

void ModrinthModItemWidget::setDownloadPath(const QString &newDownloadPath)
{
    downloadPath_ = newDownloadPath;
}
