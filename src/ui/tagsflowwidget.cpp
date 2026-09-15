#include "tagsflowwidget.h"

#include <QLabel>
#include <QMenu>

#include "tag/tagable.h"
#include "tagchip.h"
#include "util/flowlayout.h"
#include "local/localmodmenu.h"
#include "config.hpp"

TagsFlowWidget::TagsFlowWidget(QWidget *parent) :
    QWidget(parent)
{
    setLayout(new FlowLayout);
    updateUi();
    connect(this, &QWidget::customContextMenuRequested, this, &TagsFlowWidget::on_TagsFlowWidget_customContextMenuRequested);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void TagsFlowWidget::updateUi()
{
    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        layout()->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    if(!tagableObject_) return;
    for(auto &&tag : tagableObject_->tags(Config().getShowTagCategories())){
        auto label = TagChip::create(tag, this, false, "TagsFlowWidget");
        layout()->addWidget(label);
        tagWidgets_ << label;
    }
}

void TagsFlowWidget::on_TagsFlowWidget_customContextMenuRequested(const QPoint &pos)
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
