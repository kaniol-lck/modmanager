#include "modrinthmodbrowser.h"
#include "modrinthmoditemwidget.h"
#include "ui_modrinthmoditemwidget.h"

#include <QMenu>

#include "local/localmodpath.h"
#include "modrinth/modrinthmod.h"
#include "download/downloadmanager.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"
#include "config.hpp"

ModrinthModItemWidget::ModrinthModItemWidget(ModrinthModBrowser *parent, ModrinthMod *mod) :
    QWidget(parent),
    ui(new Ui::ModrinthModItemWidget),
    browser_(parent),
    mod_(mod)
{
    ui->setupUi(this);
    ui->modName->setProperty("class", "Title");
    ui->modSummary->setProperty("class", "Description");
    ui->tagsWidget->setIconOnly(true);
    ui->tagsWidget->setMod(mod_);
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
    ui->updateDateTime->setText(tr("Updated"));
    ui->updateDateTime->setDateTime(mod->modInfo().dateModified());
    ui->createDateTime->setText(tr("Created"));
    ui->createDateTime->setDateTime(mod->modInfo().dateCreated());

    //loader type
    for(auto &&loaderType : mod_->modInfo().loaderTypes()){
        auto label = new QLabel(this);
        label->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        if(loaderType == ModLoaderType::Fabric)
            label->setText(QString(R"(<img src=":/image/fabric.png" height="22" width="22"/>)"));
        else if(loaderType == ModLoaderType::Forge)
            label->setText(QString(R"(<img src=":/image/forge.svg" height="22" width="22"/>)"));
        else
            label->setText(ModLoaderType::toString(loaderType));
        label->setToolTip(ModLoaderType::toString(loaderType));
        ui->loadersLayout->addWidget(label);
    }

    if(!mod_->modInfo().icon().isNull())
        updateIcon();

    connect(mod, &ModrinthMod::iconReady, this, &ModrinthModItemWidget::updateIcon);

    if(Config().getAutoFetchModrinthFileList()){
        mod->acquireFileList();
        connect(mod, &ModrinthMod::fileListReady, this, &ModrinthModItemWidget::updateFileList);
        ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
    } else{
        ui->downloadSpeedText->hide();
        ui->downloadButton->hide();
    }

    updateUi();
    connect(mod_, &ModrinthMod::downloadStarted, this, &ModrinthModItemWidget::onDownloadStarted);
}

ModrinthModItemWidget::~ModrinthModItemWidget()
{
    delete ui;
}

void ModrinthModItemWidget::updateIcon()
{
    ui->modIcon->setPixmap(mod_->modInfo().icon().scaled(80, 80, Qt::KeepAspectRatio));
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
        ui->downloadButton->setText(tr("Download"));
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setMenu(menu);
    }
}

void ModrinthModItemWidget::onDownloadStarted()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadButton->setVisible(true);
    ui->downloadSpeedText->setVisible(true);
    ui->downloadProgress->setVisible(true);
    connect(mod_->downloader(), &AbstractDownloader::downloadProgress, this, &ModrinthModItemWidget::onDownloadProgress);
    connect(mod_->downloader(), &AbstractDownloader::downloadSpeed, this, &ModrinthModItemWidget::onDownloadProgress);
    connect(mod_->downloader(), &AbstractDownloader::finished, this, &ModrinthModItemWidget::onDownloadFinished);
}

void ModrinthModItemWidget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setValue(bytesReceived);
    ui->downloadProgress->setMaximum(bytesTotal);
}

void ModrinthModItemWidget::onDownloadSpeed(qint64 bytesPerSec)
{
    ui->downloadSpeedText->setText(speedConvert(bytesPerSec));
}

void ModrinthModItemWidget::onDownloadFinished()
{
    ui->downloadProgress->setVisible(false);
    ui->downloadSpeedText->setText(numberConvert(mod_->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
    ui->downloadButton->setText(tr("Downloaded"));
}

void ModrinthModItemWidget::downloadFile(const ModrinthFileInfo &fileInfo)
{
    mod_->download(fileInfo, browser_->downloadPath());
}

ModrinthMod *ModrinthModItemWidget::mod() const
{
    return mod_;
}

void ModrinthModItemWidget::onDownloadPathChanged()
{
    if(mod_->modInfo().fileList().isEmpty()) return;
    auto downloadPath = browser_->downloadPath();
    bool bl = false;
    if(downloadPath)
        bl = hasFile(downloadPath, mod_);
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
    ui->tagsWidget->setVisible(config.getShowModCategory());
    ui->loaderTypes->setVisible(config.getShowModLoaderType());
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

