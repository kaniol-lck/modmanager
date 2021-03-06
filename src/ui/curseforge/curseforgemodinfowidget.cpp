#include "curseforgemoddialog.h"
#include "curseforgemodinfowidget.h"
#include "ui_curseforgemodinfowidget.h"

#include <QDesktopServices>
#include <QClipboard>
#include <QMenu>

#include "curseforgemodbrowser.h"
#include "curseforge/curseforgemod.h"
#include "util/smoothscrollbar.h"
#include "util/flowlayout.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"
#include "local/localmodpath.h"
#include "curseforge/curseforgemanager.h"

CurseforgeModInfoWidget::CurseforgeModInfoWidget(CurseforgeModBrowser *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModInfoWidget),
    browser_(parent)
{
    ui->setupUi(this);
    ui->modName->addAction(ui->actionOpen_Curseforge_Mod_Dialog);
    ui->modName->addAction(ui->actionOpen_Website_Link);
    ui->modName->addAction(ui->actionCopy_Website_Link);
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
    ui->tagsWidget->setTagableObject(mod_);
    if(!mod_) return;
    connect(this, &CurseforgeModInfoWidget::modChanged, disconnecter(
                connect(mod_, &CurseforgeMod::basicInfoReady, this, &CurseforgeModInfoWidget::updateBasicInfo),
                connect(mod_, &CurseforgeMod::descriptionReady, this, &CurseforgeModInfoWidget::updateDescription),
                connect(mod_, &CurseforgeMod::iconReady, this, &CurseforgeModInfoWidget::updateThumbnail),
                connect(mod_, &QObject::destroyed, this, [=]{ setMod(nullptr); })));

    //basic info
    updateBasicInfo();
    if(!mod->modInfo().hasBasicInfo())
        mod->acquireBasicInfo();

    //description
    updateDescription();
    if(mod->modInfo().description().isEmpty()){
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireDescription();
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

    //update thumbnail
    updateThumbnail();
    if(mod_->modInfo().icon().isNull()){
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }
}

void CurseforgeModInfoWidget::updateThumbnail()
{
    ui->modIcon->setPixmap(mod_->modInfo().icon().scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}

void CurseforgeModInfoWidget::updateDescription()
{
    ui->modDescription->setFont(qApp->font());
    auto desc = mod_->modInfo().description();
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

void CurseforgeModInfoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    ui->actionOpen_Curseforge_Mod_Dialog->trigger();
    QWidget::mouseDoubleClickEvent(event);
}

void CurseforgeModInfoWidget::on_actionOpen_Curseforge_Mod_Dialog_triggered()
{
    if(!mod_) return;
    if(mod_->parent() == browser_->manager()){
        auto dialog = new CurseforgeModDialog(browser_, mod_);
        //set parent
        mod_->setParent(dialog);
        connect(dialog, &CurseforgeModDialog::finished, this, [=]{
            if(browser_->manager()->mods().contains(mod_))
                mod_->setParent(browser_->manager());
        });
        dialog->show();
    }
}

void CurseforgeModInfoWidget::on_actionOpen_Website_Link_triggered()
{
    if(!mod_) return;
    QDesktopServices::openUrl(mod_->modInfo().websiteUrl());
}


void CurseforgeModInfoWidget::on_actionCopy_Website_Link_triggered()
{
    if(!mod_) return;
    QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
}
