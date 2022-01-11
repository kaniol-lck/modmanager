#include "modrinthfilelistwidget.h"
#include "modrinthmodbrowser.h"
#include "ui_modrinthfilelistwidget.h"

#include <QStandardItemModel>

#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "local/localmodpath.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"
#include "ui/downloadpathselectmenu.h"

ModrinthFileListWidget::ModrinthFileListWidget(ModrinthModBrowser *parent) :
    ModrinthFileListWidget(static_cast<QWidget *>(parent))
{
    browser_ = parent;
}

ModrinthFileListWidget::ModrinthFileListWidget(QWidget *parent, LocalMod *localMod)
{
    setLocalMod(localMod);
}

ModrinthFileListWidget::ModrinthFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthFileListWidget),
    model_(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListView->setProperty("class", "ModList");
    connect(ui->fileListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &ModrinthFileListWidget::updateIndexWidget);
    ui->downloadPathSelect->hide();
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
    connect(this, &ModrinthFileListWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::fileListReady, this, &ModrinthFileListWidget::updateFileList)));
}

void ModrinthFileListWidget::updateUi()
{

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
    for(int i = 0; i < mod_->modInfo().fileList().size(); i++){
        //load no more than kLoadSize mods
        auto item = new QStandardItem;
        model_->appendRow(item);
        auto &&fileInfo = mod_->modInfo().fileList().at(i);
        item->setData(fileInfo.fileDate(), Qt::UserRole);
        item->setData(i, Qt::UserRole + 1);
        item->setSizeHint(QSize(0, 106));
    }
    model_->setSortRole(Qt::UserRole);
    model_->sort(0, Qt::DescendingOrder);
    ui->fileListView->setCursor(Qt::ArrowCursor);
}

void ModrinthFileListWidget::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
}

void ModrinthFileListWidget::setLocalMod(LocalMod *newLocalMod)
{
    localMod_ = newLocalMod;
    if(localMod_){
        if(!downloadPathSelectMenu_){
            downloadPathSelectMenu_ = new DownloadPathSelectMenu(this);
            ui->downloadPathSelect->setDefaultAction(downloadPathSelectMenu_->menuAction());
            ui->downloadPathSelect->setPopupMode(QToolButton::InstantPopup);
        }
        downloadPathSelectMenu_->setDownloadPath(localMod_->path());
        ui->downloadPathSelect->show();
    }
}

void ModrinthFileListWidget::setBrowser(ModrinthModBrowser *newBrowser)
{
    browser_ = newBrowser;
}

DownloadPathSelectMenu *ModrinthFileListWidget::downloadPathSelectMenu() const
{
    return browser_? browser_->downloadPathSelectMenu() : downloadPathSelectMenu_;
}

void ModrinthFileListWidget::updateIndexWidget()
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
        auto &&fileInfo = mod_->modInfo().fileList().at(item->data(Qt::UserRole + 1).toInt());
        auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo);
        ui->fileListView->setIndexWidget(model_->indexFromItem(item), itemWidget);
        item->setSizeHint(QSize(0, itemWidget->height()));
    }
}
