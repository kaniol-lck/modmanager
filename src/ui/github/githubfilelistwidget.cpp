#include "githubfilelistwidget.h"
#include "ui_githubfilelistwidget.h"

#include <QStandardItem>

#include "githubfileitemwidget.h"
#include "github/githubrelease.h"
#include "util/smoothscrollbar.h"

GitHubFileListWidget::GitHubFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GitHubFileListWidget),
    model_(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListView->setProperty("class", "ModList");
    connect(ui->fileListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &GitHubFileListWidget::updateIndexWidget);
}

GitHubFileListWidget::~GitHubFileListWidget()
{
    delete ui;
}

void GitHubFileListWidget::setRelease(GitHubRelease *release)
{
    release_= release;
    emit modChanged();

    ui->fileListView->setVisible(release_);
    if(!release_) return;

    updateFileList();
}

void GitHubFileListWidget::updateFileList()
{
    model_->clear();
    for(int i = 0; i < release_->info().assets().size(); i++){
        auto item = new QStandardItem;
        model_->appendRow(item);
        auto &&fileInfo = release_->info().assets().at(i);
//        item->setData(fileInfo.fileDate(), Qt::UserRole);
        item->setData(i, Qt::UserRole + 1);
        item->setSizeHint(QSize(0, 100));
    }
//    model_->setSortRole(Qt::UserRole);
//    model_->sort(0, Qt::DescendingOrder);
    ui->fileListView->setCursor(Qt::ArrowCursor);
    updateIndexWidget();
}

void GitHubFileListWidget::updateIndexWidget()
{
    auto beginRow = ui->fileListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->fileListView->indexAt(QPoint(0, ui->fileListView->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, 0);
        if(ui->fileListView->indexWidget(index)) continue;
        auto item = model_->item(row);
        auto &&fileInfo = release_->info().assets().at(item->data(Qt::UserRole + 1).toInt());
        auto itemWidget = new GitHubFileItemWidget(this, fileInfo);
//        itemWidget->setDownloadPath(downloadPath_);
//        connect(this, &CurseforgeFileListWidget::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
        ui->fileListView->setIndexWidget(model_->indexFromItem(item), itemWidget);
        item->setSizeHint(QSize(0, itemWidget->height()));
    }
}

void GitHubFileListWidget::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
}
