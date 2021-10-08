#ifndef LOCALMODFILTER_H
#define LOCALMODFILTER_H

#include <QObject>

class LocalMod;
class UnclosedMenu;
class QAction;

class LocalModFilter : public QObject
{
    Q_OBJECT
public:
    LocalModFilter(QWidget *parent);
    UnclosedMenu *menu() const;
    void showAll();

    bool willShow(LocalMod *mod, const QString searchText) const;
private:
    UnclosedMenu *menu_;
    UnclosedMenu *websiteMenu_;
    UnclosedMenu *typeTagMenu_;
    UnclosedMenu *functionalityTagMenu_;
    QAction *disableAction_;
    QAction *showAllAction_;
};

#endif // LOCALMODFILTER_H