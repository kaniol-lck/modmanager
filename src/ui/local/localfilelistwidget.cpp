#include "localfilelistwidget.h"
#include "ui_localfilelistwidget.h"

#include <QStandardItemModel>
#include <QButtonGroup>
#include <QRadioButton>

#include "localmoditemwidget.h"
#include "localmodfileitemwidget.h"
#include "local/localmod.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

LocalFileListWidget::LocalFileListWidget(QWidget *parent) :
    DockWidgetContent(parent),
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

void LocalFileListWidget::setMods(QList<LocalMod *> mods)
{
    if(mods == mods_) return;
    mods_ = mods;
    emit modChanged();

    ui->fileListView->setVisible(!mods.isEmpty());
    switch (mods.size()) {
    case 0:
        break;
    case 1:{
        mod_ = mods.first();
        onModFileUpdated();
        connect(this, &LocalFileListWidget::modChanged, this, disconnecter(
                    connect(mod_, &LocalMod::modFileUpdated, this, &LocalFileListWidget::onModFileUpdated)));
        break;
    }
    default:
        model_->clear();
        //TODO: laggy
        for(const auto &mod : qAsConst(mods)){
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
        }
    }

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

void LocalFileListWidget::updateIndexWidget()
{
    if(mods_.size() <= 1) return;
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
//        qDebug() << "new widget at row" << row;
        auto item = model_->item(row);
        if(!item) continue;
        auto mod = item->data().value<LocalMod*>();
        if(mod){
            auto modItemWidget = new LocalModItemWidget(ui->fileListView, mod);
            ui->fileListView->setIndexWidget(index, modItemWidget);
            item->setSizeHint(QSize(0, modItemWidget->height()));
        }
    }
}

void LocalFileListWidget::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
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
