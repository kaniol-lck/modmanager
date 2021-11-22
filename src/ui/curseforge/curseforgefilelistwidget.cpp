#include "curseforgefilelistwidget.h"
#include "ui_curseforgefilelistwidget.h"

#include <QStandardItemModel>

#include "curseforgefileitemwidget.h"
#include "curseforge/curseforgemod.h"
#include "local/localmodpath.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

CurseforgeFileListWidget::CurseforgeFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileListWidget),
    model_(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    connect(ui->fileListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeFileListWidget::onSliderChanged);
}

CurseforgeFileListWidget::~CurseforgeFileListWidget()
{
    delete ui;
}

void CurseforgeFileListWidget::setMod(CurseforgeMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->fileListView->setVisible(mod_);
    if(!mod_) return;

    updateFileList();
    if(mod_->modInfo().allFileList().isEmpty()){
        ui->fileListView->setCursor(Qt::BusyCursor);
        mod->acquireAllFileList();
    }
    connect(this, &CurseforgeFileListWidget::modChanged, this, disconnecter(
                connect(mod, &CurseforgeMod::allFileListReady, this, &CurseforgeFileListWidget::updateFileList)));
}

void CurseforgeFileListWidget::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}

void CurseforgeFileListWidget::updateUi()
{
//    for(int i = 0; i < model_->rowCount(); i++){
//        qDebug() << i;
//        auto index = model_->index(kLoadSize, 0);
//        if(auto widget = ui->fileListView->indexWidget(index)){
//            qobject_cast<CurseforgeFileItemWidget*>(widget)->updateUi();
//            qDebug() << "update:" << i;
//        }
//    }
}

void CurseforgeFileListWidget::updateFileList()
{
    model_->clear();
    int height = 0;
    for(int i = 0; i < mod_->modInfo().allFileList().size(); i++){
        auto item = new QStandardItem;
        model_->appendRow(item);
        auto &&fileInfo = mod_->modInfo().allFileList().at(i);
        item->setData(fileInfo.fileDate(), Qt::UserRole);
        item->setData(i, Qt::UserRole + 1);
    }
    model_->setSortRole(Qt::UserRole);
    model_->sort(0, Qt::DescendingOrder);
    for(int i = 0; i < model_->rowCount(); i++){
        //load no more than kLoadSize mods
        if(i == kLoadSize) break;
        auto item = model_->item(i);
        auto &&fileInfo = mod_->modInfo().allFileList().at(item->data(Qt::UserRole + 1).toInt());
        auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeFileListWidget::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
        ui->fileListView->setIndexWidget(model_->indexFromItem(item), itemWidget);
        height = itemWidget->height();
    }
    for(int i = 0; i < model_->rowCount(); i++){
        auto item = model_->item(i);
        item->setSizeHint(QSize(0, height));
    }
    ui->fileListView->setCursor(Qt::ArrowCursor);
}

void CurseforgeFileListWidget::onSliderChanged(int i[[maybe_unused]])
{
    if(model_->rowCount() < kLoadSize) return;
    if(auto index = model_->index(kLoadSize, 0); ui->fileListView->indexWidget(index)) return;
    for(int i = kLoadSize; i < model_->rowCount(); i++){
        auto item = model_->item(i);
        auto &&fileInfo = mod_->modInfo().allFileList().at(item->data(Qt::UserRole + 1).toInt());
        auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeFileListWidget::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
        ui->fileListView->setIndexWidget(model_->indexFromItem(item), itemWidget);
        item->setSizeHint(QSize(0, itemWidget->height()));
    }
}
