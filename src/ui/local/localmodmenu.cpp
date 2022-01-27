#include "localmodmenu.h"

#include <QMenu>
#include <QInputDialog>

#include "local/localmod.h"
#include "local/localmodpath.h"
#include "tag/tag.h"
#include "config.hpp"

QMenu *LocalModMenu::addTagMenu(QWidget *parent, LocalMod *mod)
{
    auto menu = new QMenu(QObject::tr("Add tag"), parent);
    menu->setIcon(QIcon::fromTheme("tag"));
    //Type category
    auto addTypeTagmenu = menu->addMenu(QIcon::fromTheme("tag"), QObject::tr("Type tag"));
    for(const auto &tag : Tag::typeTags())
        addTypeTagmenu->addAction(QIcon::fromTheme("tag"), tag.name(), [=]{
            mod->addTag(tag);
        });
    //Functionality category
    auto addFunctionalityTagmenu = menu->addMenu(QIcon::fromTheme("tag"), QObject::tr("Functionality tag"));
    auto &&tagManager = mod->path()->containedTags();
    for(auto &&tag : Config().getRightClickTagMenu()? tagManager.tags(TagCategory::FunctionalityCategory) : Tag::functionalityTags().values())
        addFunctionalityTagmenu->addAction(QIcon::fromTheme("tag"), tag.name(), [=]{
            mod->addTag(tag);
        });
    if(!addFunctionalityTagmenu->isEmpty())
        addFunctionalityTagmenu->addSeparator();
    addFunctionalityTagmenu->addAction(QIcon::fromTheme("tag-new"), QObject::tr("New functionality tag..."), [=]{
        bool ok;
        auto name = QInputDialog::getText(parent, QObject::tr("New tag"), QObject::tr("Functionality:"), QLineEdit::Normal, "", &ok);
        if(ok && !name.isEmpty())
            mod->addTag(Tag(name, TagCategory::FunctionalityCategory));
    });
    //TODO: other categories
    //Translation category
    menu->addAction(QIcon::fromTheme("tag-edit"), QObject::tr("Translation tag"), [=]{
        bool ok;
        QString str;
        if(auto translationTag = mod->tags(TagCategory::TranslationCategory); !translationTag.isEmpty())
            str = translationTag.first().name();
        auto name = QInputDialog::getText(parent, QObject::tr("Translation tag"), QObject::tr("Translation:"), QLineEdit::Normal, str, &ok);
        if(ok && !name.isEmpty()){
            mod->removeTags(TagCategory::TranslationCategory);
            mod->addTag(Tag(name, TagCategory::TranslationCategory));
        }
    });
    //Notation category
    menu->addAction(QIcon::fromTheme("tag-edit"), QObject::tr("Notation tag"), [=]{
        bool ok;
        QString str;
        if(auto notationTag = mod->tags(TagCategory::NotationCategory); !notationTag.isEmpty())
            str = notationTag.first().name();
        auto name = QInputDialog::getText(parent, QObject::tr("Notation tag"), QObject::tr("Notation:"), QLineEdit::Normal, str, &ok);
        if(ok && !name.isEmpty()){
            mod->removeTags(TagCategory::NotationCategory);
            mod->addTag(Tag(name, TagCategory::NotationCategory));
        }
    });
    //Custom category
    for(auto &&tag : Tag::customTags())
        menu->addAction(QIcon::fromTheme("tag"), tag.name(), [=]{
            mod->addTag(tag);
        });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("tag-new"), QObject::tr("New custom tag..."), [=]{
        bool ok;
        auto name = QInputDialog::getText(parent, QObject::tr("New tag"), QObject::tr("New tag name:"), QLineEdit::Normal, "", &ok);
        if(ok)
            mod->addTag(Tag(name));
    });
    return menu;
}

QMenu *LocalModMenu::removeTagmenu(QWidget *parent, LocalMod *mod)
{
    auto menu = new QMenu(QObject::tr("Remove tag"), parent);
    menu->setIcon(QIcon::fromTheme("tag-delete"));
    for(auto &&tag : mod->tags(TagCategory::CustomizableCategories))
        menu->addAction(QIcon::fromTheme("tag-delete"), tag.name(), [=]{
            mod->removeTag(tag);
        });
    return menu;
}
