#include "modrinthmodinfowidget.h"
#include "ui_modrinthmodinfowidget.h"

#include <QMenu>

#include "modrinth/modrinthmod.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"

ModrinthModInfoWidget::ModrinthModInfoWidget(QWidget *parent) :
    DockWidgetContent(parent),
    ui(new Ui::ModrinthModInfoWidget)
{
    ui->setupUi(this);
    ui->scrollArea->setVisible(false);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
}

ModrinthModInfoWidget::~ModrinthModInfoWidget()
{
    delete ui;
}

void ModrinthModInfoWidget::setMod(ModrinthMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->scrollArea->setVisible(mod_);
    ui->tagsWidget->setMod(mod_);
    if(!mod_) return;

//    auto action = new QAction(tr("Copy website link"), this);
//    connect(action, &QAction::triggered, this, [=]{
//        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
//    });
//    ui->websiteButton->addAction(action);

    updateBasicInfo();

    //update full info
    updateFullInfo();
    if(mod_->modInfo().description().isEmpty()){
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod_->acquireFullInfo();
    }

    connect(this, &ModrinthModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::fullInfoReady, this, &ModrinthModInfoWidget::updateFullInfo)));
}

void ModrinthModInfoWidget::updateBasicInfo()
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
    if(!mod_->modInfo().author().isEmpty()){
//            ui->modAuthors->setText(mod->modInfo().author());
//            ui->modAuthors->setVisible(true);
//            ui->author_label->setVisible(true);
    } else{
//            ui->modAuthors->setVisible(false);
//            ui->author_label->setVisible(false);
    }

    //update icon
    //included by basic info
    updateIcon();
    if(mod_->modInfo().iconBytes().isEmpty()){
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }
    connect(this, &ModrinthModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::iconReady, this, &ModrinthModInfoWidget::updateIcon)));
}

void ModrinthModInfoWidget::updateFullInfo()
{
    updateBasicInfo();
    auto text = mod_->modInfo().description();
    text.replace(QRegularExpression("<br ?/?>"), "\n");
//        ui->websiteButton->setVisible(!mod_->modInfo().websiteUrl().isEmpty());
    ui->modDescription->setMarkdown(text);
    ui->modDescription->setCursor(Qt::ArrowCursor);

//        //update file list
//        auto updateFileList = [=]{
//            ui->fileListWidget->clear();
//            auto files = mod->modInfo().fileList();
//            for(const auto &fileInfo : qAsConst(files)){
//                auto *listItem = new DateTimeSortItem();
//                listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
//                listItem->setSizeHint(QSize(500, 90));
//                auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo, localMod_);
//                itemWidget->setDownloadPath(downloadPath_);
//                connect(this, &ModrinthModDialog::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
//                ui->fileListWidget->addItem(listItem);
//                ui->fileListWidget->setItemWidget(listItem, itemWidget);
//            }
//            ui->fileListWidget->sortItems(Qt::DescendingOrder);
//            ui->fileListWidget->setCursor(Qt::ArrowCursor);
//        };

//        if(!mod->modInfo().fileList().isEmpty())
//            updateFileList();
//        else {
//            ui->fileListWidget->setCursor(Qt::BusyCursor);
//            mod->acquireFileList();
//            connect(mod, &ModrinthMod::fileListReady, this, updateFileList);
//        }
}

void ModrinthModInfoWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}

void ModrinthModInfoWidget::on_modSummary_customContextMenuRequested(const QPoint &pos)
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

