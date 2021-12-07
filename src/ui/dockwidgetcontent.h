#ifndef DOCKWIDGETCONTENT_H
#define DOCKWIDGETCONTENT_H

#include <QWidget>
#include <QAction>

class DockWidgetContent : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidgetContent(QWidget *parent = nullptr);

    static QAction *lockPanelsAction;

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
};

#endif // DOCKWIDGETCONTENT_H
