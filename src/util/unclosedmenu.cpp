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

void UnclosedMenu::mouseReleaseEvent(QMouseEvent *event)
{
    if(auto action = actionAt(event->pos())){
        //TODO
        connect(qobject_cast<UnclosedMenu*>(action->menu()), &UnclosedMenu::menuTriggered, this, &UnclosedMenu::menuTriggered, Qt::UniqueConnection);
        action->activate(QAction::Trigger);
        emit menuTriggered();
    }
    else
        QMenu::mouseReleaseEvent(event);
}
