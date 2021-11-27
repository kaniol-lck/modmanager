#include "localfilelistwidget.h"
#include "ui_localfilelistwidget.h"

#include <QStandardItemModel>
#include <QButtonGroup>
#include <QRadioButton>

#include "localmodfileitemwidget.h"
#include "local/localmod.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

LocalFileListWidget::LocalFileListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalFileListWidget),
    model_(new QStandardItemModel(this)),
    buttons_(new QButtonGroup(this))
{
    ui->setupUi(this);
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    connect(buttons_, &QButtonGroup::idClicked, this, &LocalFileListWidget::idClicked);
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

    onModFileUpdated();
    connect(this, &LocalFileListWidget::modChanged, this, disconnecter(
                connect(mod_, &LocalMod::modFileUpdated, this, &LocalFileListWidget::onModFileUpdated)));
}

void LocalFileListWidget::idClicked(int id)
{
    for(auto i = 0; i < model_->rowCount(); i++){
        auto item = model_->item(i);
        if(item->data(Qt::UserRole + 2).toInt() == id){
            auto file = item->data().value<LocalModFile*>();
            if(mod_->oldFiles().contains(file)){
                mod_->rollback(file);
            }
            return;
        }
    }
}

void LocalFileListWidget::onModFileUpdated()
{
    //update file list
    model_->clear();
    for(auto &&button : buttons_->buttons())
        buttons_->removeButton(button);
    int id = 0;
    addModFile(mod_->modFile(), id++, true);
    for(auto file : mod_->oldFiles())
        addModFile(file, id++);
    for(auto file : mod_->duplicateFiles())
        addModFile(file, id++);
    model_->setSortRole(Qt::UserRole);
    model_->sort(0, Qt::DescendingOrder);
}

void LocalFileListWidget::addModFile(LocalModFile *file, int id, bool checked)
{
    auto item = new QStandardItem;
    model_->appendRow(item);
    item->setData(QVariant::fromValue(file));
    item->setData(file->commonInfo()->version(), Qt::UserRole);
    item->setData(id, Qt::UserRole + 2);
    auto widget = new LocalModFileItemWidget(this, file);
    auto button = widget->button();
    buttons_->addButton(button, id);
    if(checked) button->setChecked(true);
    ui->fileListView->setIndexWidget(model_->indexFromItem(item), widget);
    item->setSizeHint(QSize(0, widget->height()));
}
