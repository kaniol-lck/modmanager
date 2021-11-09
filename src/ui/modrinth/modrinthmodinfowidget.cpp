#include "modrinthmodinfowidget.h"
#include "ui_modrinthmodinfowidget.h"

#include "modrinth/modrinthmod.h"
#include "util/flowlayout.h"

ModrinthModInfoWidget::ModrinthModInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthModInfoWidget)
{
    ui->setupUi(this);
    ui->tagsWidget->setLayout(new FlowLayout());
    ui->scrollArea->setVisible(false);
}

ModrinthModInfoWidget::~ModrinthModInfoWidget()
{
    delete ui;
}

void ModrinthModInfoWidget::setMod(ModrinthMod *mod)
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

    auto updateBasicInfo = [=]{
        setWindowTitle(mod->modInfo().name() + tr(" - Modrinth"));
        ui->modName->setText(mod->modInfo().name());
//        ui->modSummary->setText(mod->modInfo().summary());
        if(!mod->modInfo().author().isEmpty()){
//            ui->modAuthors->setText(mod->modInfo().author());
//            ui->modAuthors->setVisible(true);
//            ui->author_label->setVisible(true);
        } else{
//            ui->modAuthors->setVisible(false);
//            ui->author_label->setVisible(false);
        }

        //update icon
        //included by basic info
        auto updateIcon= [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod->modInfo().iconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!mod->modInfo().iconBytes().isEmpty())
            updateIcon();
        else {
            mod->acquireIcon();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(mod, &ModrinthMod::iconReady, this, updateIcon);
        }
    };

    auto bl = mod->modInfo().hasBasicInfo();
    if(bl) updateBasicInfo();

    //update full info
    auto updateFullInfo = [=]{
        if(!bl) updateBasicInfo();
        auto text = mod->modInfo().description();
        text.replace(QRegExp("<br ?/?>"), "\n");
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
    };

    if(!mod->modInfo().description().isEmpty())
        updateFullInfo();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireFullInfo();
        //TODO: disconnect when mod changed
        connect(mod, &ModrinthMod::fullInfoReady, this, updateFullInfo);
    }
}
