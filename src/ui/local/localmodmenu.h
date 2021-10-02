#ifndef LOCALMODMENU_H
#define LOCALMODMENU_H

#include <QObject>

class LocalMod;
class QMenu;

class LocalModMenu : public QObject
{
    Q_OBJECT
public:
    explicit LocalModMenu(QWidget *parent, LocalMod *mod);
    QMenu *addTagMenu() const;
    QMenu *removeTagmenu() const;

private:
    LocalMod *mod_;
};

#endif // LOCALMODMENU_H
