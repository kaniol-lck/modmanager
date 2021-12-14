#include "tagswidget.h"
#include "ui_tagswidget.h"

#include <QLabel>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QMenu>

#include "tag/tagable.h"
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
        auto label = new QLabel(this);
        if(!tag.iconName().isEmpty())
            if(iconOnly_)
                label->setText(QString(R"(<img src="%1" height="16" width="16"/>)").arg(tag.iconName()));
            else
                label->setText(QString(R"(<img src="%1" height="16" width="16"/> %2)").arg(tag.iconName(), tag.name()));
        else
            label->setText(tag.name());
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        if(!iconOnly_)
            label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        ui->tagsLayout->addWidget(label);
        tagWidgets_ << label;
    }
}

void TagsWidget::wheelEvent(QWheelEvent *event)
{
    auto &&scrollBar = ui->scrollArea->horizontalScrollBar();
    auto value = scrollBar->value() - event->angleDelta().y() * 0.2;
    scrollBar->setValue(value);
    if(scrollBar->value())
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
        auto localModMenu = new LocalModMenu(this, mod);
        menu->addMenu(localModMenu->addTagMenu());
        if(!mod->tags(TagCategory::CustomizableCategories).isEmpty())
            menu->addMenu(localModMenu->removeTagmenu());
        menu->exec(mapToGlobal(pos));
    }
}

