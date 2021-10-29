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
    using QMenu::addMenu;
    QAction *addMenu(UnclosedMenu *menu);
    bool isUnclosed() const;
public slots:
    void setUnclosed(bool unclosed);
signals:
    void menuTriggered();
protected:
    void mouseReleaseEvent(QMouseEvent *event);
private:
    bool isUnclosed_ = true;
};

#endif // UNCLOSEDMENU_H
