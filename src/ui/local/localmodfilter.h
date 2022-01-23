#ifndef LOCALMODFILTER_H
#define LOCALMODFILTER_H

#include <QMap>
#include <QObject>

#include "tag/tagcategory.h"

class LocalMod;
class LocalModPath;
class UnclosedMenu;
class QAction;

class LocalModFilter : public QObject
{
    Q_OBJECT
public:
    LocalModFilter(QWidget *parent, LocalModPath *path);
    UnclosedMenu *menu() const;
    void showAll();

    bool willShow(LocalMod *mod, const QString searchText) const;
public slots:
    void refreshTags() const;

private:
    LocalModPath *path_;
    UnclosedMenu *menu_;
    UnclosedMenu *websiteMenu_;
    QMap<TagCategory, UnclosedMenu *> tagMenus_;
    QAction *disableAction_;
    QAction *showAllAction_;
};

#endif // LOCALMODFILTER_H
