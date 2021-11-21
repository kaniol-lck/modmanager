#include "curseforgemodinfowidget.h"
#include "ui_curseforgemodinfowidget.h"

#include <QMenu>

#include "curseforge/curseforgemod.h"
#include "util/smoothscrollbar.h"
#include "util/flowlayout.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"

CurseforgeModInfoWidget::CurseforgeModInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModInfoWidget)
{
    ui->setupUi(this);
    ui->tagsWidget->setLayout(new FlowLayout());
    ui->scrollArea->setVisible(false);
    //lag
//    ui->scrollArea->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
}

CurseforgeModInfoWidget::~CurseforgeModInfoWidget()
{
    delete ui;
}

void CurseforgeModInfoWidget::setMod(CurseforgeMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->scrollArea->setVisible(mod_);
    if(!mod_) return;

//    auto action = new QAction(tr("Copy website link"), this);
//    connect(action, &QAction::triggered, this, [=]{
//        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
//    });
//    ui->websiteButton->addAction(action);

    //basic info
    updateBasicInfo();
    if(!mod->modInfo().hasBasicInfo())
        mod->acquireBasicInfo();
    connect(this, &CurseforgeModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &CurseforgeMod::basicInfoReady, this, &CurseforgeModInfoWidget::updateBasicInfo)));

    //description
    updateDescription();
    if(mod->modInfo().description().isEmpty()){
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireDescription();
    }
    connect(this, &CurseforgeModInfoWidget::modChanged, this, disconnecter(
                connect(mod, &CurseforgeMod::descriptionReady, this, &CurseforgeModInfoWidget::updateDescription)));

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
}

void CurseforgeModInfoWidget::updateBasicInfo()
{
    ui->modName->setText(mod_->modInfo().name());
    ui->modSummary->setText(mod_->modInfo().summary());
    if(Config().getAutoTranslate()){
        YoudaoTranslator::translator()->translate(mod_->modInfo().summary(), [=](const auto &translted){
            if(!translted.isEmpty())
                ui->modSummary->setText(translted);
            transltedSummary_ = true;
        });
    }
//    ui->modAuthors->setText(mod_->modInfo().authors().join(", "));

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
            label->setText(QString(R"(<img src="%1" height="16" width="16"/> %2)").arg(tag.iconName(), tag.name()));
        else
            label->setText(tag.name());
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        ui->tagsWidget->layout()->addWidget(label);
        tagWidgets_ << label;
    }

    //update thumbnail
    updateThumbnail();
    if(mod_->modInfo().iconBytes().isEmpty()){
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }
    connect(this, &CurseforgeModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &CurseforgeMod::iconReady, this, &CurseforgeModInfoWidget::updateThumbnail)));
}

void CurseforgeModInfoWidget::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}

void CurseforgeModInfoWidget::updateDescription()
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

void CurseforgeModInfoWidget::on_modSummary_customContextMenuRequested(const QPoint &pos)
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

