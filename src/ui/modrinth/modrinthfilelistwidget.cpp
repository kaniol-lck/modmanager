#include "modrinthfilelistwidget.h"
#include "ui_modrinthfilelistwidget.h"

#include <QStandardItemModel>

#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "local/localmodpath.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

ModrinthFileListWidget::ModrinthFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthFileListWidget),
    model_(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    connect(ui->fileListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &ModrinthFileListWidget::onSliderChanged);
}

ModrinthFileListWidget::~ModrinthFileListWidget()
{
    delete ui;
}

void ModrinthFileListWidget::setMod(ModrinthMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->fileListView->setVisible(mod_);
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
        ui->fileListView->setCursor(Qt::BusyCursor);
        mod_->acquireFileList();
    }
}

void ModrinthFileListWidget::updateFileList()
{
    model_->clear();
    int height = 0;
    for(auto &&fileInfo : mod_->modInfo().fileList()){
        //load no more than kLoadSize mods
        auto item = new QStandardItem;
        model_->appendRow(item);
        item->setData(fileInfo.fileDate(), Qt::UserRole);
    }
    model_->setSortRole(Qt::UserRole);
    model_->sort(0, Qt::DescendingOrder);
    for(int i = 0; i < model_->rowCount(); i++){
        //load no more than kLoadSize mods
        if(i == kLoadSize) break;
        auto &&fileInfo = mod_->modInfo().fileList().at(i);
        auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &ModrinthFileListWidget::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
        ui->fileListView->setIndexWidget(model_->indexFromItem(model_->item(i)), itemWidget);
        height = itemWidget->height();
    }
    for(int i = 0; i < model_->rowCount(); i++){
        auto item = model_->item(i);
        item->setSizeHint(QSize(0, height));
    }
    ui->fileListView->setCursor(Qt::ArrowCursor);
}

void ModrinthFileListWidget::onSliderChanged(int i[[maybe_unused]])
{
    if(model_->rowCount() < kLoadSize) return;
    if(auto index = model_->index(kLoadSize, 0); ui->fileListView->indexWidget(index)) return;
    for(int i = kLoadSize; i < model_->rowCount(); i++){
        auto &&fileInfo = mod_->modInfo().fileList().at(i);
        auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &ModrinthFileListWidget::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
        auto item = model_->item(i);
        ui->fileListView->setIndexWidget(model_->indexFromItem(item), itemWidget);
        item->setSizeHint(QSize(0, itemWidget->height()));
    }
}
