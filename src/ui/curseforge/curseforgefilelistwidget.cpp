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

void CurseforgeFileListWidget::updateFileList()
{
    ui->fileListView->setVisible(false);
    model_->clear();
    auto files = mod_->modInfo().allFileList();
    for(const auto &fileInfo : files){
        auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeFileListWidget::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
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
