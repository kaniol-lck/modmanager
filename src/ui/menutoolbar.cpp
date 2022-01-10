#include "menutoolbar.h"

#include <QMenu>

MenuToolBar::MenuToolBar(QWidget *parent) :
    QToolBar(parent)
{}

QAction *MenuToolBar::addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode mode)
{
    auto button = new QToolButton(this);
    button->setDefaultAction(menu->menuAction());
    button->setToolButtonStyle(toolButtonStyle());
    button->setPopupMode(mode);
    return addWidget(button);
}
