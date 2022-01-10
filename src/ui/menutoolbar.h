#ifndef MENUTOOLBAR_H
#define MENUTOOLBAR_H

#include <QToolBar>
#include <QToolButton>

class MenuToolBar : public QToolBar
{
public:
    MenuToolBar(QWidget *parent);

    QAction *addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode mode = QToolButton::InstantPopup);
};

#endif // MENUTOOLBAR_H
