#ifndef UNCLOSEDMENU_H
#define UNCLOSEDMENU_H

#include <QMenu>

class UnclosedMenu : public QMenu
{
    Q_OBJECT
public:
    UnclosedMenu(QWidget *parent = nullptr);
    UnclosedMenu(const QString &title, QWidget *parent = nullptr);
    ~UnclosedMenu() = default;
signals:
    void menuTriggered();
protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // UNCLOSEDMENU_H
