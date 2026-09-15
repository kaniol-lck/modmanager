#include "tagswidget.h"
#include "ui_tagswidget.h"

#include <QLabel>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QMenu>

#include "tag/tagable.h"
#include "tagchip.h"
#include "local/localmodmenu.h"
#include "config.hpp"

TagsWidget::TagsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagsWidget)
{
    ui->setupUi(this);
    ui->scrollArea->verticalScrollBar()->setEnabled(false);
}

TagsWidget::~TagsWidget()
{
    delete ui;
}

void TagsWidget::updateUi()
{
    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        ui->tagsLayout->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    if(!tagableObject_) return;
    for(auto &&tag : tagableObject_->tags(Config().getShowTagCategories())){
        auto label = TagChip::create(tag, this, iconOnly_, "TagsWidget");
        // AlignVCenter：芯片已定死高度，不加对齐会被 QHBoxLayout 纵向拉伸
        ui->tagsLayout->addWidget(label, 0, Qt::AlignVCenter);
        tagWidgets_ << label;
    }
}

void TagsWidget::wheelEvent(QWheelEvent *event)
{
    auto &&scrollBar = ui->scrollArea->horizontalScrollBar();
    auto value = scrollBar->value() - event->angleDelta().y() * 0.2;
    scrollBar->setValue(value);
    if(scrollBar->maximum())
        event->accept();
}

bool TagsWidget::iconOnly() const
{
    return iconOnly_;
}

void TagsWidget::setIconOnly(bool newIconOnly)
{
    iconOnly_ = newIconOnly;
    updateUi();
}

void TagsWidget::on_TagsWidget_customContextMenuRequested(const QPoint &pos)
{
    if(isLocalMod_){
        auto mod = dynamic_cast<LocalMod *>(tagableObject_);
        auto menu = new QMenu(this);
        menu->addMenu(LocalModMenu::addTagMenu(this, mod));
        if(!mod->tags(TagCategory::CustomizableCategories).isEmpty())
            menu->addMenu(LocalModMenu::removeTagmenu(this, mod));
        menu->exec(mapToGlobal(pos));
    }
}

