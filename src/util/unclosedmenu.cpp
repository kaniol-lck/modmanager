#include "unclosedmenu.h"

#include <QMouseEvent>
#include <QDebug>

UnclosedMenu::UnclosedMenu(QWidget *parent) :
    QMenu(parent)
{
    connect(this, &QMenu::triggered, this, &UnclosedMenu::menuTriggered);
}

UnclosedMenu::UnclosedMenu(const QString &title, QWidget *parent) :
    QMenu(title, parent)
{
    connect(this, &QMenu::triggered, this, &UnclosedMenu::menuTriggered);
}

QAction *UnclosedMenu::addMenu(UnclosedMenu *menu)
{
    connect(menu, &UnclosedMenu::menuTriggered, this, &UnclosedMenu::menuTriggered);
    return QMenu::addMenu(menu);
}

void UnclosedMenu::mouseReleaseEvent(QMouseEvent *event)
{
    if(auto action = actionAt(event->pos())){
        action->activate(QAction::Trigger);
        emit menuTriggered();
    }
    else
        QMenu::mouseReleaseEvent(event);
}
