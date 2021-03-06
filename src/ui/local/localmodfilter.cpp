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
    disableAction_(new QAction(tr("Disabled mods"), parent)),
    showAllAction_(new QAction(tr("Show all"), parent))
{
    addSubTagable(&path_->containedTags());
    disableAction_->setCheckable(true);
    menu_->addAction(showAllAction_);
    //show all
    connect(showAllAction_, &QAction::triggered, this, [=]{
        for(auto &&action : websiteMenu_->actions())
            action->setChecked(true);
        for(const auto &menu : qAsConst(tagMenus_))
            for(auto &&action : menu->actions())
                action->setChecked(true);
        disableAction_->setChecked(true);
    });
    //hide all
    connect(menu_->addAction(tr("Hide all")), &QAction::triggered, this, [=]{
        for(auto &&action : websiteMenu_->actions())
            action->setChecked(false);
        for(const auto &menu : qAsConst(tagMenus_))
            for(auto &&action : menu->actions())
                action->setChecked(true);
        disableAction_->setChecked(false);
    });
    menu_->addSeparator();
    menu_->addMenu(websiteMenu_);
    for(const auto &category : TagCategory::FilterCategories){
        auto menu = new UnclosedMenu(category.name());
        tagMenus_.insert(category, menu);
        menu_->addMenu(menu);
    }
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

//    refreshTags();
//    connect(this, &LocalModFilter::tagsChanged, this, &LocalModFilter::refreshTags);
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
    bool showTags = true;
    //for every category
    for(auto it = tagMenus_.cbegin(); it != tagMenus_.cend(); it++){
        bool showTag = false;
        for(auto &&tag : mod->tags(it.key())){
            bool hasTag = false;
            for(auto &&action : it.value()->actions())
                if(action->text() == tag.name()){
                    hasTag = true;
                    break;
                }
            if(!hasTag){
                showTag = true;
                break;
            }
        }
        if(showTag) continue;

        //for every tag
        for(auto it2 = it.value()->actions().cbegin() + 3; it2 < it.value()->actions().cend(); it2++){
            auto &&action = *it2;
            bool hasTag = false;
            if(!action->isChecked()) continue;
            if(action->data().toBool() && mod->tags(it.key()).isEmpty()){
                showTag = true;
                break;
            }
            for(auto &&tag : mod->tags(it.key())){
                if(action->text() == tag.name()){
                    hasTag = true;
                    break;
                }
            }
            if(hasTag) {
                showTag = true;
                break;
            }
        }
        if(!showTag){
//            qDebug() << mod->displayName() << it.value()->menuAction()->text() << "not satisfy";
            showTags = false;
            break;
        }
    }
//    qDebug() << show << showWebsite << showTags;
    return show && showWebsite && showTags;
}

void LocalModFilter::refreshTags() const
{
    qDebug() << "refreshTags";
    auto addTags = [=](UnclosedMenu *menu, const QList<Tag> &tags){
        QMap<QString, bool> map;
        for(auto &&action : menu->actions())
            map[action->text()] = action->isChecked();
        menu->clear();
        menu->addAction(tr("Show all"), this, [=]{
            for(auto &&action : menu->actions())
                action->setChecked(true);
        });
        menu->addAction(tr("Hide all"), this, [=]{
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
    for(const auto &category : TagCategory::FilterCategories)
        addTags(tagMenus_[category], tags(category));
}
