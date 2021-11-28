#include "curseforgemoddialog.h"
#include "ui_curseforgemoddialog.h"

#include <QDir>
#include <QDesktopServices>
#include <QAction>
#include <QClipboard>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QMenu>

#include "local/localmodpath.h"
#include "curseforge/curseforgemod.h"
#include "curseforgefileitemwidget.h"
#include "curseforgeimagepopup.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/flowlayout.h"
#include "util/youdaotranslator.h"
#include "download/qaria2.h"

CurseforgeModDialog::CurseforgeModDialog(QWidget *parent, CurseforgeMod *mod, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModDialog),
    model_(new QStandardItemModel(this)),
    mod_(mod),
    localMod_(localMod)
{
    ui->setupUi(this);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->galleryListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    auto action = new QAction(tr("Copy website link"), this);
    connect(action, &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
    });
    ui->websiteButton->addAction(action);

    connect(mod_, &CurseforgeMod::destroyed, this, &QDialog::close);

    //update basic info
    if(mod_->modInfo().hasBasicInfo())
        updateBasicInfo();
    else
        mod_->acquireBasicInfo();

    //update description
    if(!mod_->modInfo().description().isEmpty())
        updateDescription();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod_->acquireDescription();
    }

    //update file list
    if(!mod_->modInfo().allFileList().isEmpty())
        updateFileList();
    else {
        ui->fileListView->setCursor(Qt::BusyCursor);
        mod_->acquireAllFileList();
    }

    connect(mod_, &CurseforgeMod::basicInfoReady, this, &CurseforgeModDialog::updateBasicInfo);
    connect(mod_, &CurseforgeMod::iconReady, this, &CurseforgeModDialog::updateThumbnail);
    connect(mod_, &CurseforgeMod::descriptionReady, this, &CurseforgeModDialog::updateDescription);
    connect(mod_, &CurseforgeMod::allFileListReady, this, &CurseforgeModDialog::updateFileList);

    if(localMod_) setDownloadPath(localMod_->path());
}

CurseforgeModDialog::~CurseforgeModDialog()
{
    delete ui;
}

void CurseforgeModDialog::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}

void CurseforgeModDialog::updateBasicInfo()
{
    setWindowTitle(mod_->modInfo().name() + tr(" - Curseforge"));
    ui->modName->setText(mod_->modInfo().name());
    ui->modSummary->setText(mod_->modInfo().summary());
    if(Config().getAutoTranslate()){
        YoudaoTranslator::translator()->translate(mod_->modInfo().summary(), [=](const auto &translted){
            if(!translted.isEmpty())
                ui->modSummary->setText(translted);
            transltedSummary_ = true;
        });
    }
    ui->modAuthors->setText(mod_->modInfo().authors().join(", "));

    //update thumbnail
    //included by basic info
    if(!mod_->modInfo().iconBytes().isEmpty())
        updateThumbnail();
    else {
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }

    //update gallery
//    if(mod_->modInfo().images().isEmpty())
//        ui->tabWidget->removeTab(1);
    for(const auto &image : mod_->modInfo().images()){
        auto item = new QListWidgetItem();
        item->setText(image.title);
        item->setToolTip(image.description);
        item->setData(Qt::UserRole, image.url);
        item->setData(Qt::UserRole + 1, image.title);
        item->setData(Qt::UserRole + 2, image.description);
        item->setSizeHint(QSize(260, 260));
        ui->galleryListWidget->addItem(item);
        QNetworkRequest request(image.thumbnailUrl);
        static QNetworkAccessManager accessManager;
        static QNetworkDiskCache diskCache;
        diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        accessManager.setCache(&diskCache);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        auto reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, [=]{
            if(reply->error() != QNetworkReply::NoError) return;
            auto bytes = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(bytes);
            item->setIcon(QIcon(pixmap));
            item->setData(Qt::UserRole + 3, bytes);
            reply->deleteLater();
        });
    }
}

void CurseforgeModDialog::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}

void CurseforgeModDialog::updateDescription()
{
    ui->modDescription->setFont(qApp->font());
    auto desc = mod_->modInfo().description();
//        QRegExp re(R"(<img.*src=\"(.+)\")");
//        re.setMinimal(true);
//        int pos = 0;
//        QStringList replceList;
//        while ((pos = re.indexIn(desc, pos)) != -1) {
//            replceList << re.cap(1);
//            pos +=  re.matchedLength();
//        }
//        QDir().mkdir("image_cache");
//        for(const auto &str : qAsConst(replceList)){
//            qDebug() << str;
//            QUrl url(str);
//            auto fileName = url.fileName();
//            QFileInfo fileInfo("./image_cache/" + fileName);
//            if(fileInfo.exists()) continue;
//            auto downloader = QAria2::qaria2()->download(url, "./image_cache/");
//            connect(downloader, &QAria2Downloader::finished, ui->modDescription, &QTextBrowser::reload);
//            desc.replace(str, "./image_cache/" + fileName);
//        }
    ui->modDescription->setHtml(desc);
    ui->modDescription->setCursor(Qt::ArrowCursor);
}

void CurseforgeModDialog::updateFileList()
{
    ui->fileListView->setVisible(false);
    model_->clear();
    auto files = mod_->modInfo().allFileList();
    for(const auto &fileInfo : files){
        auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo, localMod_);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeModDialog::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
        auto item = new QStandardItem;
        item->setData(fileInfo.fileDate(), Qt::UserRole);
        item->setSizeHint(QSize(0, itemWidget->height()));
        model_->appendRow(item);
        ui->fileListView->setIndexWidget(model_->indexFromItem(item), itemWidget);
    }
    ui->fileListView->setVisible(true);
    model_->setSortRole(Qt::UserRole);
    model_->sort(0, Qt::DescendingOrder);
    ui->fileListView->setCursor(Qt::ArrowCursor);
}

void CurseforgeModDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().websiteUrl());
}

void CurseforgeModDialog::on_galleryListWidget_itemClicked(QListWidgetItem *item)
{
    auto popup = new CurseforgeImagePopup(
                this,
                item->data(Qt::UserRole).toUrl(),
                item->data(Qt::UserRole + 1).toString(),
                item->data(Qt::UserRole + 2).toString(),
                item->data(Qt::UserRole + 3).toByteArray());
    popup->exec();
}


void CurseforgeModDialog::on_modSummary_customContextMenuRequested(const QPoint &pos)
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

