#include "localmodfilter.h"

#include <QDebug>
#include <QApplication>

#include "tag/tag.h"
#include "util/unclosedmenu.h"
#include "local/localmod.h"
#include "local/localmodpath.h"

LocalModFilter::LocalModFilter(QWidget *parent, LocalModPath *path) :
    QObject(parent),
    path_(path),
    menu_(new UnclosedMenu(parent)),
    websiteMenu_(new UnclosedMenu(tr("Website source"), parent)),
    environmentTagMenu_(new UnclosedMenu(tr("Environment tag"), parent)),
    typeTagMenu_(new UnclosedMenu(tr("Type tag"), parent)),
    functionalityTagMenu_(new UnclosedMenu(tr("Functionality tag"), parent)),
    customTagMenu_(new UnclosedMenu(tr("Custom tag"), parent)),
    disableAction_(new QAction(tr("Disabled mods"), parent)),
    showAllAction_(new QAction(tr("Show all"), parent))
{
    disableAction_->setCheckable(true);
    menu_->addAction(showAllAction_);
    //show all
    connect(showAllAction_, &QAction::triggered, this, [=]{
        for(auto &&action : websiteMenu_->actions())
            action->setChecked(true);
        for(auto &&action : environmentTagMenu_->actions())
            action->setChecked(true);
        for(auto &&action : typeTagMenu_->actions())
            action->setChecked(true);
        for(auto &&action : functionalityTagMenu_->actions())
            action->setChecked(true);
        for(auto &&action : customTagMenu_->actions())
            action->setChecked(true);
        disableAction_->setChecked(true);
    });
    //hide all
    connect(menu_->addAction(tr("Hide all")), &QAction::triggered, this, [=]{
        for(auto &&action : websiteMenu_->actions())
            action->setChecked(false);
        for(auto &&action : environmentTagMenu_->actions())
            action->setChecked(false);
        for(auto &&action : typeTagMenu_->actions())
            action->setChecked(false);
        for(auto &&action : functionalityTagMenu_->actions())
            action->setChecked(false);
        for(auto &&action : customTagMenu_->actions())
            action->setChecked(false);
        disableAction_->setChecked(false);
    });
    menu_->addSeparator();
    menu_->addMenu(websiteMenu_);
    menu_->addMenu(environmentTagMenu_);
    menu_->addMenu(typeTagMenu_);
    menu_->addMenu(functionalityTagMenu_);
    menu_->addMenu(customTagMenu_);
    menu_->addAction(disableAction_);

    //website
    connect(websiteMenu_->addAction(tr("Show all")), &QAction::triggered, this, [=]{
        for(auto &&action : websiteMenu_->actions())
            action->setChecked(true);
    });
    connect(websiteMenu_->addAction(tr("Hide all")), &QAction::triggered, this, [=]{
        for(auto &&action : websiteMenu_->actions())
            action->setChecked(false);
    });
    websiteMenu_->addSeparator();
    websiteMenu_->addAction(QIcon(":/image/curseforge.svg"), "Curseforge")->setCheckable(true);
    websiteMenu_->addAction(QIcon(":/image/modrinth.svg"), "Modrinth")->setCheckable(true);
    auto noneAction = websiteMenu_->addAction(tr("None"));
    noneAction->setCheckable(true);
    noneAction->setData(true);
    //environment tag
    connect(environmentTagMenu_->addAction(tr("Show all")), &QAction::triggered, this, [=]{
        for(auto &&action : environmentTagMenu_->actions())
            action->setChecked(true);
    });
    connect(environmentTagMenu_->addAction(tr("Hide all")), &QAction::triggered, this, [=]{
        for(auto &&action : environmentTagMenu_->actions())
            action->setChecked(false);
    });
    environmentTagMenu_->addSeparator();
    for(auto &&tag : Tag::enironmentTags()){
        auto action = environmentTagMenu_->addAction(tag.name());
        action->setCheckable(true);
    }
    noneAction = environmentTagMenu_->addAction(tr("None"));
    noneAction->setCheckable(true);
    noneAction->setData(true);
    //type tag
    connect(typeTagMenu_->addAction(tr("Show all")), &QAction::triggered, this, [=]{
        for(auto &&action : typeTagMenu_->actions())
            action->setChecked(true);
    });
    connect(typeTagMenu_->addAction(tr("Hide all")), &QAction::triggered, this, [=]{
        for(auto &&action : typeTagMenu_->actions())
            action->setChecked(false);
    });
    typeTagMenu_->addSeparator();
    for(auto &&tag : Tag::typeTags()){
        auto action = typeTagMenu_->addAction(tag.name());
        action->setCheckable(true);
    }
    noneAction = typeTagMenu_->addAction(tr("None"));
    noneAction->setCheckable(true);
    noneAction->setData(true);

    connect(menu_, &QMenu::aboutToShow, this, &LocalModFilter::refreshTags);
}

UnclosedMenu *LocalModFilter::menu() const
{
    return menu_;
}

void LocalModFilter::showAll()
{
    showAllAction_->trigger();
}

bool LocalModFilter::willShow(LocalMod *mod, const QString searchText) const
{
    bool show = true;
    if(!(mod->commonInfo()->name().toLower().contains(searchText) ||
         mod->commonInfo()->description().toLower().contains(searchText) ||
         mod->commonInfo()->id().toLower().contains(searchText)))
        show = false;
    if(!disableAction_->isChecked() && mod->isDisabled())
        show = false;
    bool showWebsite = false;
    for(auto &&action : websiteMenu_->actions()){
        if(action->text() == "Curseforge" && action->isChecked() && mod->curseforgeMod())
            showWebsite = true;
        if(action->text() == "Modrinth" && action->isChecked() && mod->modrinthMod())
            showWebsite = true;
        if(action->data().toBool() && action->isChecked() && !mod->curseforgeMod() && !mod->modrinthMod())
            showWebsite = true;
    }
    bool showEnvironmentTag = false;
    for(auto &&action : environmentTagMenu_->actions()){
        bool hasTag = false;
        if(!action->isChecked()) continue;
        if(action->data().toBool() && mod->tagManager().environmentTags().isEmpty()){
            showEnvironmentTag = true;
            break;
        }
        for(auto &&tag : mod->tagManager().environmentTags()){
            if(action->text() == tag.name()){
                hasTag = true;
                break;
            }
        }
        if(hasTag) {
            showEnvironmentTag = true;
            break;
        }
    }
    bool showTypeTag = false;
    for(auto &&action : typeTagMenu_->actions()){
        bool hasTag = false;
        if(!action->isChecked()) continue;
        if(action->data().toBool() && mod->tagManager().typeTags().isEmpty()){
            showTypeTag = true;
            break;
        }
        for(auto &&tag : mod->tagManager().typeTags()){
            if(action->text() == tag.name()){
                hasTag = true;
                break;
            }
        }
        if(hasTag) {
            showTypeTag = true;
            break;
        }
    }
    bool showFunctionalityTag = false;
    for(auto &&action : functionalityTagMenu_->actions()){
        bool hasTag = false;
        if(!action->isChecked()) continue;
        if(action->data().toBool() && mod->tagManager().functionalityTags().isEmpty()){
            showFunctionalityTag = true;
            break;
        }
        for(auto &&tag : mod->tagManager().functionalityTags()){
            if(action->text() == tag.name()){
                hasTag = true;
                break;
            }
        }
        if(hasTag) {
            showFunctionalityTag = true;
            break;
        }
    }
    bool showCustomTag = false;
    for(auto &&action : customTagMenu_->actions()){
        bool hasTag = false;
        if(!action->isChecked()) continue;
        if(action->data().toBool() && mod->tagManager().customTags().isEmpty()){
            showCustomTag = true;
            break;
        }
        for(auto &&tag : mod->tagManager().customTags()){
            if(action->text() == tag.name()){
                hasTag = true;
                break;
            }
        }
        if(hasTag) {
            showCustomTag = true;
            break;
        }
    }
    return show && showWebsite && showEnvironmentTag && showTypeTag && showFunctionalityTag && showCustomTag;
}

void LocalModFilter::refreshTags() const
{
    auto addTags = [=](UnclosedMenu *menu, const QList<Tag> &tags){
        QMap<QString, bool> map;
        for(auto &&action : menu->actions())
            map[action->text()] = action->isChecked();
        menu->clear();
        connect(menu->addAction(tr("Show all")), &QAction::triggered, this, [=]{
            for(auto &&action : menu->actions())
                action->setChecked(true);
        });
        connect(menu->addAction(tr("Hide all")), &QAction::triggered, this, [=]{
            for(auto &&action : menu->actions())
                action->setChecked(false);
        });
        menu->addSeparator();
        for(auto &&tag : tags){
            auto action = menu->addAction(tag.name());
            action->setCheckable(true);
            if(map.contains(tag.name()))
                action->setChecked(map[tag.name()]);
            else
                action->setChecked(true);
        }
        auto noneAction = menu->addAction(tr("None"));
        noneAction->setCheckable(true);
        if(map.contains(tr("None")))
            noneAction->setChecked(map[tr("None")]);
        else
            noneAction->setChecked(true);
        noneAction->setData(true);
    };
    auto &&tagManager = path_->tagManager();
    //functionality tag
    addTags(functionalityTagMenu_, tagManager.functionalityTags());
    //custom tag
    addTags(customTagMenu_, tagManager.customTags());
}
