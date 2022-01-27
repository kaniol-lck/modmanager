#ifndef LOCALMODMENU_H
#define LOCALMODMENU_H

class QWidget;
class LocalMod;
class QMenu;

namespace LocalModMenu {
    QMenu *addTagMenu(QWidget *parent, LocalMod *mod);
    QMenu *removeTagmenu(QWidget *parent, LocalMod *mod);
};

#endif // LOCALMODMENU_H
