#include "curseforgemodbrowser.h"
#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include <QDebug>
#include <QMenu>

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "download/downloadmanager.h"
#include "download/qaria2downloader.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"

#include <ui/downloadpathselectmenu.h>

CurseforgeModItemWidget::CurseforgeModItemWidget(CurseforgeModBrowser *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    browser_(parent),
    mod_(mod),
    defaultFileInfo_(defaultDownload)
{
    ui->setupUi(this);
    ui->modName->setProperty("class", "Title");
    ui->modSummary->setProperty("class", "Description");
    ui->downloadProgress->setVisible(false);
    ui->tagsWidget->setIconOnly(true);
    ui->tagsWidget->setMod(mod_);
    updateIcon();
    connect(mod_, &CurseforgeMod::iconReady, this, &CurseforgeModItemWidget::updateIcon);
    onDownloadPathChanged();
    connect(browser_->downloadPathSelectMenu(), &DownloadPathSelectMenu::DownloadPathChanged, this, &CurseforgeModItemWidget::onDownloadPathChanged);

    auto menu = new QMenu(this);

    if(defaultFileInfo_){
        auto name = defaultFileInfo_.value().displayName() + " ("+ sizeConvert(defaultFileInfo_.value().size()) + ")";
        connect(menu->addAction(QIcon::fromTheme("starred-symbolic"), name), &QAction::triggered, this, [=]{
            downloadFile(*defaultFileInfo_);
        });

        if(!mod->modInfo().latestFiles().isEmpty())
            menu->addSeparator();
    }

    for(const auto &fileInfo : mod->modInfo().latestFiles()){
        auto name = fileInfo.displayName() + " ("+ sizeConvert(fileInfo.size()) + ")";
        connect(menu->addAction(name), &QAction::triggered, this, [=]{
            downloadFile(fileInfo);
        });
    }

    ui->downloadButton->setMenu(menu);

    ui->modName->setText(mod->modInfo().name());
    ui->modSummary->setText(mod->modInfo().summary());
    if(Config().getAutoTranslate()){
        YoudaoTranslator::translator()->translate(mod->modInfo().summary(), [=](const auto &translted){
            if(!translted.isEmpty())
                ui->modSummary->setText(translted);
            transltedSummary_ = true;
        });
    }
    ui->modAuthors->setText(mod->modInfo().authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
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

    if(defaultFileInfo_.has_value())
        ui->downloadSpeedText->setText(sizeConvert(defaultDownload.value().size()) + "\n"
                                       + numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));
    else
        ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));

    updateUi();
    connect(mod_, &CurseforgeMod::downloadStarted, this, &CurseforgeModItemWidget::onDownloadStarted);
}

CurseforgeModItemWidget::~CurseforgeModItemWidget()
{
    delete ui;
}

void CurseforgeModItemWidget::updateIcon()
{
    ui->modIcon->setPixmap(mod_->modInfo().icon().scaled(80, 80, Qt::KeepAspectRatio));
}

void CurseforgeModItemWidget::onDownloadStarted()
{
    ui->downloadButton->setText(tr("Downloading"));
    ui->downloadButton->setEnabled(false);
    ui->downloadProgress->setVisible(true);
    connect(mod_->downloader(), &AbstractDownloader::downloadProgress, this, &CurseforgeModItemWidget::onDownloadProgress);
    connect(mod_->downloader(), &AbstractDownloader::downloadSpeed, this, &CurseforgeModItemWidget::onDownloadProgress);
    connect(mod_->downloader(), &AbstractDownloader::finished, this, &CurseforgeModItemWidget::onDownloadFinished);

}

void CurseforgeModItemWidget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->downloadProgress->setValue(bytesReceived);
    ui->downloadProgress->setMaximum(bytesTotal);
}

void CurseforgeModItemWidget::onDownloadSpeed(qint64 bytesPerSec)
{
    ui->downloadSpeedText->setText(speedConvert(bytesPerSec));
}

void CurseforgeModItemWidget::onDownloadFinished()
{
    ui->downloadProgress->setVisible(false);
//    ui->downloadSpeedText->setText(sizeConvert(fileInfo.size()));
    ui->downloadButton->setText(tr("Downloaded"));
}

void CurseforgeModItemWidget::downloadFile(const CurseforgeFileInfo &fileInfo)
{
    mod_->download(fileInfo, browser_->downloadPath());
}

CurseforgeMod *CurseforgeModItemWidget::mod() const
{
    return mod_;
}

void CurseforgeModItemWidget::onDownloadPathChanged()
{
    bool bl = false;
    if(auto downloadPath = browser_->downloadPath())
        bl = hasFile(downloadPath, mod_);
    else{
        if(defaultFileInfo_)
            bl = hasFile(Config().getDownloadPath(), defaultFileInfo_->fileName());
        if(!mod_->modInfo().latestFiles().isEmpty())
            for(auto &&fileInfo : mod_->modInfo().latestFiles()){
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

void CurseforgeModItemWidget::updateUi()
{
    Config config;
    ui->modAuthors->setVisible(config.getShowModAuthors());
    ui->modDateTime->setVisible(config.getShowModDateTime());
    ui->tagsWidget->setVisible(config.getShowModCategory());
    ui->loaderTypes->setVisible(config.getShowModLoaderType());
}

void CurseforgeModItemWidget::on_modSummary_customContextMenuRequested(const QPoint &pos)
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

