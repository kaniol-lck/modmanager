#include "modrinthfilelistwidget.h"
#include "ui_modrinthfilelistwidget.h"

#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "local/localmodpath.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

ModrinthFileListWidget::ModrinthFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthFileListWidget)
{
    ui->setupUi(this);
    ui->fileListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
}

ModrinthFileListWidget::~ModrinthFileListWidget()
{
    delete ui;
}

void ModrinthFileListWidget::setMod(ModrinthMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->fileListWidget->setVisible(mod_);
    if(!mod_) return;

    //update full info
    updateFullInfo();
    if(mod_->modInfo().description().isEmpty()){
        mod_->acquireFullInfo();
    }

    connect(this, &ModrinthFileListWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::fullInfoReady, this, &ModrinthFileListWidget::updateFullInfo)));
}

void ModrinthFileListWidget::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}

void ModrinthFileListWidget::updateFullInfo()
{
    //update file list
    updateFileList();
    if(mod_->modInfo().fileList().isEmpty()){
        ui->fileListWidget->setCursor(Qt::BusyCursor);
        mod_->acquireFileList();
    }
}

void ModrinthFileListWidget::updateFileList()
{
    ui->fileListWidget->clear();
    auto files = mod_->modInfo().fileList();
    for(const auto &fileInfo : qAsConst(files)){
        auto *listItem = new DateTimeSortItem();
        listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
        listItem->setSizeHint(QSize(0, 90));
        auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &ModrinthFileListWidget::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
        ui->fileListWidget->addItem(listItem);
        ui->fileListWidget->setItemWidget(listItem, itemWidget);
    }
    ui->fileListWidget->sortItems(Qt::DescendingOrder);
    ui->fileListWidget->setCursor(Qt::ArrowCursor);
}
