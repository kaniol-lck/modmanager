#ifndef UNCLOSEDMENU_H
#define UNCLOSEDMENU_H

#include <QMenu>

class UnclosedMenu : public QMenu
{
public:
    UnclosedMenu(QWidget *parent = nullptr);
    UnclosedMenu(const QString &title, QWidget *parent = nullptr);
    ~UnclosedMenu() = default;
protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // UNCLOSEDMENU_H
