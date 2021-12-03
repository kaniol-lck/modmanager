#include "tagswidget.h"
#include "ui_tagswidget.h"

#include "local/localmod.h"
#include "config.hpp"

#include <QLabel>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>

TagsWidget::TagsWidget(QWidget *parent, LocalMod* mod) :
    QWidget(parent),
    ui(new Ui::TagsWidget),
    mod_(mod)
{
    ui->setupUi(this);
    ui->scrollArea->verticalScrollBar()->setEnabled(false);
    updateUi();
    if(mod_) connect(mod, &LocalMod::modFileUpdated, this, &TagsWidget::updateUi);
}

void TagsWidget::setMod(LocalMod *mod)
{
    if(mod_) disconnect(mod, &LocalMod::modFileUpdated, this, &TagsWidget::updateUi);
    mod_ = mod;
    updateUi();
    connect(mod, &LocalMod::modFileUpdated, this, &TagsWidget::updateUi);
}

void TagsWidget::updateUi()
{
    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        ui->tagsLayout->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    if(!mod_) return;
    for(auto &&tag : mod_->tags(Config().getShowTagCategories())){
        auto label = new QLabel(tag.name(), this);
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
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
    if(scrollBar->value() == value)
        event->accept();
}
