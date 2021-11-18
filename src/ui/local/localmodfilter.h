#ifndef LOCALMODFILTER_H
#define LOCALMODFILTER_H

#include <QObject>

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
    UnclosedMenu *subDirTagMenu_;
    UnclosedMenu *environmentTagMenu_;
    UnclosedMenu *typeTagMenu_;
    UnclosedMenu *functionalityTagMenu_;
    UnclosedMenu *customTagMenu_;
    QAction *disableAction_;
    QAction *showAllAction_;
};

#endif // LOCALMODFILTER_H
