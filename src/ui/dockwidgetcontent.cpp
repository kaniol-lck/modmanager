#include "dockwidgetcontent.h"

#include <QMenu>

QAction *DockWidgetContent::lockPanelsAction = nullptr;

DockWidgetContent::DockWidgetContent(QWidget *parent) : QWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &DockWidgetContent::onCustomContextMenuRequested);
}

void DockWidgetContent::onCustomContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    menu->addAction(DockWidgetContent::lockPanelsAction);
    menu->exec(mapToGlobal(pos));
}
