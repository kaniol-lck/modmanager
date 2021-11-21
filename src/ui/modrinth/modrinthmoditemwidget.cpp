#include "modrinthmoditemwidget.h"
#include "ui_modrinthmoditemwidget.h"

#include <QMenu>

#include "local/localmodpath.h"
#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"
#include "config.hpp"

ModrinthModItemWidget::ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod) :
    QWidget(parent),
    ui(new Ui::ModrinthModItemWidget),
    mod_(mod)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);
    ui->downloadButton->setEnabled(false);

    ui->modName->setText(mod->modInfo().name());
    ui->modSummary->setText(mod->modInfo().summary());
    if(Config().getAutoTranslate()){
        YoudaoTranslator::translator()->translate(mod_->modInfo().summary(), [=](const auto &translted){
            if(!translted.isEmpty())
                ui->modSummary->setText(translted);
            transltedSummary_ = true;
        });
    }
    ui->modAuthors->setText("by <b>" + mod->modInfo().author() + "</b>");
    ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
    ui->modUpdateDate->setText(tr("%1 ago").arg(timesTo(mod->modInfo().dateModified())));
    ui->modUpdateDate->setToolTip(mod->modInfo().dateModified().toString());
    ui->modCreateDate->setText(tr("%1 ago").arg(timesTo(mod->modInfo().dateCreated())));
    ui->modCreateDate->setToolTip(mod->modInfo().dateCreated().toString());

    //tags
    for(auto &&tag : mod_->tags()){
        auto label = new QLabel(this);
        if(!tag.iconName().isEmpty())
            // a bit smaller than curseforge's
            label->setText(QString(R"(<img src="%1" height="16" width="16"/>)").arg(tag.iconName()));
        else
            label->setText(tag.name());
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        if(tag.category() != TagCategory::ModrinthCategory)
            label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        ui->tagsLayout->addWidget(label);
    }

    mod->acquireFileList();

    connect(mod, &ModrinthMod::iconReady, this, &ModrinthModItemWidget::updateIcon);
    connect(mod, &ModrinthMod::fileListReady, this, &ModrinthModItemWidget::updateFileList);

    updateUi();
}

ModrinthModItemWidget::~ModrinthModItemWidget()
{
    delete ui;
}

void ModrinthModItemWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
}

void ModrinthModItemWidget::updateFileList()
{
    auto menu = new QMenu(this);
    auto addInfo = [=](const auto &fileInfo){
        auto name = fileInfo.displayName()/* + " ("+ numberConvert(fileInfo.size(), "B") + ")"*/;
        connect(menu->addAction(name), &QAction::triggered, this, [=]{
            downloadFile(fileInfo);
        });
    };

    auto featuredFileList = mod_->modInfo().featuredFileList();
    if(!featuredFileList.isEmpty()){
        for(const auto &fileInfo : featuredFileList)
            addInfo(fileInfo);
    } else {
        auto fileList = mod_->modInfo().fileList();
        auto count = 0;
        for(const auto &fileInfo : qAsConst(fileList)){
            addInfo(fileInfo);
            if(++count == 2) break;
        }
    }

    if(!menu->actions().isEmpty()){
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setMenu(menu);
    }
    setDownloadPath(downloadPath_);
}

void ModrinthModItemWidget::downloadFile(const ModrinthFileInfo &fileInfo)
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);

    QAria2Downloader *downloader;
    DownloadFileInfo info(fileInfo);
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    info.setIcon(pixelmap);
    info.setTitle(mod_->modInfo().name());
    if(downloadPath_)
        downloader = downloadPath_->downloadNewMod(info);
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
        ui->downloadSpeedText->setText(numberConvert(mod_->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
        ui->downloadButton->setText(tr("Downloaded"));
    });
}

ModrinthMod *ModrinthModItemWidget::mod() const
{
    return mod_;
}

void ModrinthModItemWidget::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;

    if(mod_->modInfo().fileList().isEmpty()) return;
    bool bl = false;
    if(downloadPath_)
        bl = hasFile(downloadPath_, mod_);
    else if(!mod_->modInfo().fileList().isEmpty())
        for(const auto &fileInfo : mod_->modInfo().fileList()){
            if(hasFile(Config().getDownloadPath(), fileInfo.fileName())){
                bl = true;
                break;
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

void ModrinthModItemWidget::updateUi()
{
    Config config;
    ui->modAuthors->setVisible(config.getShowModAuthors());
    ui->modDateTime->setVisible(config.getShowModDateTime());
    ui->tags->setVisible(config.getShowModCategory());
}

void ModrinthModItemWidget::on_modSummary_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    if(!transltedSummary_)
        menu->addAction(tr("Translate summary"), this, [=]{
            YoudaoTranslator::translator()->translate(mod_->modInfo().summary(), [=](const QString &translated){
                if(!translated.isEmpty()){
                    ui->modSummary->setText(translated);
                transltedSummary_ = true;
                }
            });
        });
    else{
        transltedSummary_ = false;
        menu->addAction(tr("Untranslate summary"), this, [=]{
            ui->modSummary->setText(mod_->modInfo().summary());
        });
    }
    menu->exec(ui->modSummary->mapToGlobal(pos));
}

