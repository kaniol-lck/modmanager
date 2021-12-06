#include "tagsflowwidget.h"

#include <QLabel>

#include "local/localmod.h"
#include "util/flowlayout.h"

TagsFlowWidget::TagsFlowWidget(QWidget *parent, LocalMod* mod) :
    QWidget(parent),
    mod_(mod)
{
    setLayout(new FlowLayout);
    updateUi();
    if(mod_) connect(mod, &LocalMod::tagsChanged, this, &TagsFlowWidget::updateUi);
}

void TagsFlowWidget::setMod(LocalMod *mod)
{
    if(mod_) disconnect(mod, &LocalMod::tagsChanged, this, &TagsFlowWidget::updateUi);
    mod_ = mod;
    updateUi();
    connect(mod, &LocalMod::tagsChanged, this, &TagsFlowWidget::updateUi);
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
        auto label = new QLabel(tag.name(), this);
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        layout()->addWidget(label);
        tagWidgets_ << label;
    }
}
