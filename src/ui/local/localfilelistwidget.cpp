#include "localfilelistwidget.h"
#include "ui_localfilelistwidget.h"

#include <QStandardItemModel>

#include "localmodfileitemwidget.h"
#include "local/localmod.h"
#include "util/smoothscrollbar.h"

LocalFileListWidget::LocalFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalFileListWidget),
    model_(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
}

LocalFileListWidget::~LocalFileListWidget()
{
    delete ui;
}

void LocalFileListWidget::setMod(LocalMod *mod)
{
    if(mod == mod_) return;
    mod_ = mod;
    emit modChanged();

    ui->fileListView->setVisible(mod_);
    if(!mod_) return;

    //update file list
    model_->clear();
    addModFile(mod_->modFile());
    for(auto file : mod_->oldFiles())
        addModFile(file);
    for(auto file : mod_->duplicateFiles())
        addModFile(file);
    model_->setSortRole(Qt::UserRole + 1);
    model_->sort(0);
}

void LocalFileListWidget::addModFile(LocalModFile *file)
{
    auto item = new QStandardItem;
    model_->appendRow(item);
    item->setData(file->commonInfo()->version());
    auto widget = new LocalModFileItemWidget(this, file);
    ui->fileListView->setIndexWidget(model_->indexFromItem(item), widget);
    item->setSizeHint(QSize(0, widget->height()));
}
