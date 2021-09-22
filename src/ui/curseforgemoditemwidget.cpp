#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include <QDebug>
#include <QMenu>

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "download/downloadmanager.h"
#include "download/downloader.h"
#include "util/funcutil.h"

CurseforgeModItemWidget::CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    mod_(mod),
    defaultFileInfo_(defaultDownload)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    connect(mod_, &CurseforgeMod::iconReady, this, &CurseforgeModItemWidget::updateIcon);

    auto menu = new QMenu(this);

    if(defaultFileInfo_){
        auto name = defaultFileInfo_.value().displayName() + " ("+ numberConvert(defaultFileInfo_.value().size(), "B") + ")";
        connect(menu->addAction(QIcon::fromTheme("starred-symbolic"), name), &QAction::triggered, this, [=]{
            downloadFile(*defaultFileInfo_);
        });

        if(!mod->modInfo().latestFileList().isEmpty())
            menu->addSeparator();
    }

    for(const auto &fileInfo : mod->modInfo().latestFileList()){
        auto name = fileInfo.displayName() + " ("+ numberConvert(fileInfo.size(), "B") + ")";
        connect(menu->addAction(name), &QAction::triggered, this, [=]{
            downloadFile(fileInfo);
        });
    }

    ui->downloadButton->setMenu(menu);

    ui->modName->setText(mod->modInfo().name());
    ui->modSummary->setText(mod->modInfo().summary());
    ui->modAuthors->setText(mod->modInfo().authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    if(defaultFileInfo_.has_value())
        ui->downloadSpeedText->setText(numberConvert(defaultDownload.value().size(), "B") + "\n"
                                       + numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
    else
        ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
}

CurseforgeModItemWidget::~CurseforgeModItemWidget()
{
    delete ui;
}

void CurseforgeModItemWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
}

void CurseforgeModItemWidget::downloadFile(const CurseforgeFileInfo &fileInfo)
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    DownloadFileInfo info(fileInfo);
    if(downloadPath_)
        info.setPath(downloadPath_->info().path());
    else
        info.setPath(Config().getDownloadPath());
    info.setIconBytes(mod_->modInfo().iconBytes());

    auto downloader = DownloadManager::addModDownload(info);

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

CurseforgeMod *CurseforgeModItemWidget::mod() const
{
    return mod_;
}

void CurseforgeModItemWidget::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;

    bool bl = false;
    if(downloadPath_)
        bl = hasFile(downloadPath_, mod_);
    else{
        if(defaultFileInfo_)
            bl = hasFile(Config().getDownloadPath(), defaultFileInfo_->fileName());
        if(!mod_->modInfo().latestFileList().isEmpty())
            for(const auto &fileInfo : mod_->modInfo().latestFileList()){
                if(hasFile(Config().getDownloadPath(), fileInfo.fileName())){
                    bl = true;
                    break;
                }
            }
    }
    if(bl){
        ui->downloadButton->setEnabled(false);
        ui->downloadButton->setText(tr("Downloaded"));
    } else{
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setText(tr("Download"));
    }
}
