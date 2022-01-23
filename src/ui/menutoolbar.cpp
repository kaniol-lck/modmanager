#include "menutoolbar.h"

#include <QMenu>

MenuToolBar::MenuToolBar(QWidget *parent) :
    QToolBar(parent)
{}

QAction *MenuToolBar::addMenu(QAction *action, QToolButton::ToolButtonPopupMode mode)
{
    auto button = new QToolButton(this);
    button->setDefaultAction(action);
    button->setToolButtonStyle(toolButtonStyle());
    button->setPopupMode(mode);
    return addWidget(button);
}

QAction *MenuToolBar::addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode mode)
{
    return addMenu(menu->menuAction(), mode);
}

QAction *MenuToolBar::insertMenu(QAction *before, QAction *action, QToolButton::ToolButtonPopupMode mode)
{
    auto button = new QToolButton(this);
    button->setDefaultAction(action);
    button->setToolButtonStyle(toolButtonStyle());
    button->setPopupMode(mode);
    return insertWidget(before, button);
}

QAction *MenuToolBar::insertMenu(QAction *before, QMenu *menu, QToolButton::ToolButtonPopupMode mode)
{
    return insertMenu(before, menu->menuAction(), mode);
}
