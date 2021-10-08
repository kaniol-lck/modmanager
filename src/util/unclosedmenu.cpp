#include "unclosedmenu.h"

#include <QMouseEvent>

UnclosedMenu::UnclosedMenu(QWidget *parent) :
    QMenu(parent)
{}

UnclosedMenu::UnclosedMenu(const QString &title, QWidget *parent) :
    QMenu(title, parent)
{}

void UnclosedMenu::mouseReleaseEvent(QMouseEvent *event)
{
    if(auto action = actionAt(event->pos()))
        action->activate(QAction::Trigger);
    else
        QMenu::mouseReleaseEvent(event);
}
