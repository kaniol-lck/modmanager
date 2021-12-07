#include "tagsflowwidget.h"

#include <QLabel>

#include "tag/tagable.h"
#include "util/flowlayout.h"
#include "config.hpp"

TagsFlowWidget::TagsFlowWidget(QWidget *parent) :
    QWidget(parent)
{
    setLayout(new FlowLayout);
    updateUi();
}

void TagsFlowWidget::updateUi()
{
    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        layout()->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    if(!mod_) return;
    for(auto &&tag : mod_->tags(Config().getShowTagCategories())){
        auto label = new QLabel(this);
        if(!tag.iconName().isEmpty())
            label->setText(QString(R"(<img src="%1" height="16" width="16"/> %2)").arg(tag.iconName(), tag.name()));
        else
            label->setText(tag.name());
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        layout()->addWidget(label);
        tagWidgets_ << label;
    }
}
