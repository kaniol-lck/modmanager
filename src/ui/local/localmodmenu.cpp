#include "localmodmenu.h"

#include <QMenu>
#include <QInputDialog>

#include "local/localmod.h"
#include "local/localmodpath.h"
#include "tag/tag.h"
#include "config.hpp"

LocalModMenu::LocalModMenu(QWidget *parent, LocalMod *mod) :
    QObject(parent),
    mod_(mod)
{}

QMenu *LocalModMenu::addTagMenu() const
{
    auto widget = qobject_cast<QWidget*>(parent());
    auto menu = new QMenu(tr("Add tag"), widget);
    menu->setIcon(QIcon::fromTheme("tag"));
    //Type category
    auto addTypeTagmenu = menu->addMenu(QIcon::fromTheme("tag"), tr("Type tag"));
    for(const auto &tag : Tag::typeTags())
        connect(addTypeTagmenu->addAction(QIcon::fromTheme("tag"), tag.name()), &QAction::triggered, this, [=]{
            mod_->addTag(tag);
        });
    //Functionality category
    auto addFunctionalityTagmenu = menu->addMenu(QIcon::fromTheme("tag"), tr("Functionality tag"));
    auto &&tagManager = mod_->path()->containedTags();
    for(auto &&tag : Config().getRightClickTagMenu()? tagManager.tags(TagCategory::FunctionalityCategory) : Tag::functionalityTags().values())
        connect(addFunctionalityTagmenu->addAction(QIcon::fromTheme("tag"), tag.name()), &QAction::triggered, this, [=]{
            mod_->addTag(tag);
        });
    if(!addFunctionalityTagmenu->isEmpty())
        addFunctionalityTagmenu->addSeparator();
    connect(addFunctionalityTagmenu->addAction(QIcon::fromTheme("tag-new"), tr("New functionality tag...")), &QAction::triggered, this, [=]{
        bool ok;
        auto name = QInputDialog::getText(widget, tr("New tag"), tr("Functionality:"), QLineEdit::Normal, "", &ok);
        if(ok && !name.isEmpty())
            mod_->addTag(Tag(name, TagCategory::FunctionalityCategory));
    });
    //TODO: other categories
    //Translation category
    connect(menu->addAction(QIcon::fromTheme("tag-edit"), tr("Translation tag")), &QAction::triggered, this, [=]{
        bool ok;
        QString str;
        if(auto translationTag = mod_->tags(TagCategory::TranslationCategory); !translationTag.isEmpty())
            str = translationTag.first().name();
        auto name = QInputDialog::getText(widget, tr("Translation tag"), tr("Translation:"), QLineEdit::Normal, str, &ok);
        if(ok && !name.isEmpty()){
            mod_->removeTags(TagCategory::TranslationCategory);
            mod_->addTag(Tag(name, TagCategory::TranslationCategory));
        }
    });
    //Notation category
    connect(menu->addAction(QIcon::fromTheme("tag-edit"), tr("Notation tag")), &QAction::triggered, this, [=]{
        bool ok;
        QString str;
        if(auto notationTag = mod_->tags(TagCategory::NotationCategory); !notationTag.isEmpty())
            str = notationTag.first().name();
        auto name = QInputDialog::getText(widget, tr("Notation tag"), tr("Notation:"), QLineEdit::Normal, str, &ok);
        if(ok && !name.isEmpty()){
            mod_->removeTags(TagCategory::NotationCategory);
            mod_->addTag(Tag(name, TagCategory::NotationCategory));
        }
    });
    //Custom category
    for(auto &&tag : Tag::customTags())
        connect(menu->addAction(QIcon::fromTheme("tag"), tag.name()), &QAction::triggered, this, [=]{
            mod_->addTag(tag);
        });
    menu->addSeparator();
    connect(menu->addAction(QIcon::fromTheme("tag-new"), tr("New custom tag...")), &QAction::triggered, this, [=]{
        bool ok;
        auto name = QInputDialog::getText(widget, tr("New tag"), tr("New tag name:"), QLineEdit::Normal, "", &ok);
        if(ok)
            mod_->addTag(Tag(name));
    });
    return menu;
}

QMenu *LocalModMenu::removeTagmenu() const
{
    auto widget = qobject_cast<QWidget*>(parent());
    auto menu = new QMenu(tr("Remove tag"), widget);
    menu->setIcon(QIcon::fromTheme("tag-delete"));
    for(auto &&tag : mod_->tags(TagCategory::CustomizableCategories))
        connect(menu->addAction(QIcon::fromTheme("tag-delete"), tag.name()), &QAction::triggered, this, [=]{
            mod_->removeTag(tag);
        });
    return menu;
}
