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
    ui->fileListView->setVisible(false);
    model_->clear();
    auto files = mod_->modInfo().fileList();
    for(const auto &fileInfo : qAsConst(files)){
        auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &ModrinthFileListWidget::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
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
