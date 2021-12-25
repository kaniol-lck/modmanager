#include "localfilelistwidget.h"
#include "ui_localfilelistwidget.h"

#include <QStandardItemModel>
#include <QButtonGroup>
#include <QRadioButton>

#include "localmoditemwidget.h"
#include "localmodfileitemwidget.h"
#include "local/localmod.h"
#include "local/localmodpath.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

LocalFileListWidget::LocalFileListWidget(QWidget *parent, LocalModPath *path) :
    QWidget(parent),
    path_(path),
    ui(new Ui::LocalFileListWidget),
    updateModel_(new QStandardItemModel(this)),
    model_(new QStandardItemModel(this)),
    buttons_(new QButtonGroup(this))
{
    ui->setupUi(this);
    ui->updateFileListView->setModel(updateModel_);
    ui->updateFileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->updateFileListView->setProperty("class", "ModList");
    ui->fileListView->setModel(model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListView->setProperty("class", "ModList");
    connect(buttons_, &QButtonGroup::idClicked, this, &LocalFileListWidget::idClicked);
    if(path_){
        onUpdatesReady();
        connect(path_, &LocalModPath::updatableCountChanged, this, &LocalFileListWidget::onUpdatesReady);
        connect(path_, &LocalModPath::updatesReady, this, &LocalFileListWidget::onUpdatesReady);
    }
    ui->stackedWidget->setCurrentIndex(0);
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
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 1:
        ui->stackedWidget->setCurrentIndex(1);
        mod_ = mods.first();
        onModFileUpdated();
        connect(this, &LocalFileListWidget::modChanged, this, disconnecter(
                    connect(mod_, &LocalMod::modInfoChanged, this, &LocalFileListWidget::onModFileUpdated)));
        break;
    default:
        ui->stackedWidget->setCurrentIndex(1);
        model_->clear();
        for(const auto &mod : qAsConst(mods)){
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            item->setToolTip(mod->displayName());
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

void LocalFileListWidget::onUpdatesReady()
{
    if(auto count = path_->updatableCount()){
        ui->updateText->setText(tr("%1 mods need update.").arg(count));
        updateModel_->clear();
        for(auto &&map : path_->modMaps()) for(const auto &mod : map){
            if(mod->updateTypes().isEmpty()) continue;
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            item->setToolTip(mod->displayName());
            updateModel_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
        }
    } else{
        ui->updateText->setText("");
        updateModel_->clear();
    }
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
//            modItemWidget->setMinimal(true);
            ui->fileListView->setIndexWidget(index, modItemWidget);
            item->setSizeHint(QSize(0, modItemWidget->height()));
        }
    }
}

void LocalFileListWidget::updateUpdateIndexWidget()
{
    auto beginRow = ui->updateFileListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->updateFileListView->indexAt(QPoint(0, ui->updateFileListView->height())).row();
    if(endRow < 0)
        endRow = updateModel_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < updateModel_->rowCount(); row++){
        auto index = updateModel_->index(row, 0);
        if(ui->updateFileListView->indexWidget(index)) continue;
//        qDebug() << "new widget at row" << row;
        auto item = updateModel_->item(row);
        if(!item) continue;
        auto mod = item->data().value<LocalMod*>();
        if(mod){
            auto modItemWidget = new LocalModItemWidget(ui->updateFileListView, mod);
//            modItemWidget->setMinimal(true);
            ui->updateFileListView->setIndexWidget(index, modItemWidget);
            item->setSizeHint(QSize(0, modItemWidget->height()));
        }
    }
}

void LocalFileListWidget::paintEvent(QPaintEvent *event)
{
    if(ui->stackedWidget->currentIndex() == 0)
        updateUpdateIndexWidget();
    else
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
