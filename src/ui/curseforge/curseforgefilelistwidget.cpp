#include "curseforgefilelistwidget.h"
#include "ui_curseforgefilelistwidget.h"

#include "curseforgefileitemwidget.h"
#include "curseforge/curseforgemod.h"
#include "local/localmodpath.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

CurseforgeFileListWidget::CurseforgeFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileListWidget)
{
    ui->setupUi(this);
    ui->fileListWidget->setVerticalScrollBar(new SmoothScrollBar(this));
}

CurseforgeFileListWidget::~CurseforgeFileListWidget()
{
    delete ui;
}

void CurseforgeFileListWidget::setMod(CurseforgeMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->fileListWidget->setVisible(mod_);
    if(!mod_) return;

    updateFileList();
    if(mod_->modInfo().allFileList().isEmpty()){
        ui->fileListWidget->setCursor(Qt::BusyCursor);
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
    ui->fileListWidget->clear();
    auto files = mod_->modInfo().allFileList();
    for(const auto &fileInfo : files){
        auto *listItem = new DateTimeSortItem();
        listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
        listItem->setSizeHint(QSize(0, 90));
        auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &CurseforgeFileListWidget::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
        ui->fileListWidget->addItem(listItem);
        ui->fileListWidget->setItemWidget(listItem, itemWidget);
    }
    ui->fileListWidget->sortItems(Qt::DescendingOrder);
    ui->fileListWidget->setCursor(Qt::ArrowCursor);
}
