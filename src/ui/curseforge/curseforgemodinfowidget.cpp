#include "curseforgemodinfowidget.h"
#include "ui_curseforgemodinfowidget.h"

#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/flowlayout.h"

CurseforgeModInfoWidget::CurseforgeModInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModInfoWidget)
{
    ui->setupUi(this);
    ui->tagsWidget->setLayout(new FlowLayout());
    ui->scrollArea->setVisible(false);
}

CurseforgeModInfoWidget::~CurseforgeModInfoWidget()
{
    delete ui;
}

void CurseforgeModInfoWidget::setMod(CurseforgeMod *mod)
{
    mod_ = mod;

    ui->scrollArea->setVisible(mod_);
    if(!mod_) return;

//    auto action = new QAction(tr("Copy website link"), this);
//    connect(action, &QAction::triggered, this, [=]{
//        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
//    });
//    ui->websiteButton->addAction(action);


    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        ui->tagsWidget->layout()->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    for(auto &&tag : mod_->tags()){
        auto label = new QLabel(this);
        label->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
        if(!tag.iconName().isEmpty())
            label->setText(QString(R"(<img src="%1" height="22" width="22"/>)").arg(tag.iconName()));
        else
            label->setText(tag.name());
        label->setToolTip(tag.name());
        if(tag.tagCategory() != TagCategory::CurseforgeCategory)
            label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.tagCategory().color().name()));
        ui->tagsWidget->layout()->addWidget(label);
        tagWidgets_ << label;
    }

    //update basic info
    auto updateBasicInfo = [=]{
        setWindowTitle(mod->modInfo().name() + tr(" - Curseforge"));
        ui->modName->setText(mod->modInfo().name());
//        ui->modSummary->setText(mod->modInfo().summary());
//        ui->modAuthors->setText(mod->modInfo().authors().join(", "));

        //update thumbnail
        //included by basic info
        auto updateThumbnail = [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod_->modInfo().iconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!mod_->modInfo().iconBytes().isEmpty())
            updateThumbnail();
        else {
            mod->acquireIcon();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(mod_, &CurseforgeMod::iconReady, this, updateThumbnail);
        }
    };

    if(mod_->modInfo().hasBasicInfo())
        updateBasicInfo();
    else {
        mod->acquireBasicInfo();
        connect(mod_, &CurseforgeMod::basicInfoReady, this, updateBasicInfo);
    }

    //update description
    auto updateDescription = [=]{
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
    };

    if(!mod_->modInfo().description().isEmpty())
        updateDescription();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireDescription();
        connect(mod, &CurseforgeMod::descriptionReady, this, updateDescription);
    }

//    //update gallery
//    if(mod->modInfo().images().isEmpty())
//        ui->tabWidget->removeTab(1);
//    for(const auto &image : mod->modInfo().images()){
//        auto item = new QListWidgetItem();
//        item->setText(image.title);
//        item->setToolTip(image.description);
//        item->setData(Qt::UserRole, image.url);
//        item->setData(Qt::UserRole + 1, image.title);
//        item->setData(Qt::UserRole + 2, image.description);
//        item->setSizeHint(QSize(260, 260));
//        ui->galleryListWidget->addItem(item);
//        QNetworkRequest request(image.thumbnailUrl);
//        static QNetworkAccessManager accessManager;
//        static QNetworkDiskCache diskCache;
//        diskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
//        accessManager.setCache(&diskCache);
//        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
//        auto reply = accessManager.get(request);
//        connect(reply, &QNetworkReply::finished, this, [=]{
//            if(reply->error() != QNetworkReply::NoError) return;
//            auto bytes = reply->readAll();
//            QPixmap pixmap;
//            pixmap.loadFromData(bytes);
//            item->setIcon(QIcon(pixmap));
//            item->setData(Qt::UserRole + 3, bytes);
//            reply->deleteLater();
//        });
//    }

//    //update file list
//    auto updateFileList = [=]{
//        ui->fileListWidget->clear();
//        auto files = mod_->modInfo().allFileList();
//        for(const auto &fileInfo : files){
//            auto *listItem = new DateTimeSortItem();
//            listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
//            listItem->setSizeHint(QSize(500, 90));
//            auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo, localMod_);
//            itemWidget->setDownloadPath(downloadPath_);
//            connect(this, &CurseforgeModDialog::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
//            ui->fileListWidget->addItem(listItem);
//            ui->fileListWidget->setItemWidget(listItem, itemWidget);
//        }
//        ui->fileListWidget->sortItems(Qt::DescendingOrder);
//        ui->fileListWidget->setCursor(Qt::ArrowCursor);
//    };

//    if(!mod_->modInfo().allFileList().isEmpty())
//        updateFileList();
//    else {
//        ui->fileListWidget->setCursor(Qt::BusyCursor);
//        mod->acquireAllFileList();
//        connect(mod, &CurseforgeMod::allFileListReady, this, updateFileList);
//    }
}
