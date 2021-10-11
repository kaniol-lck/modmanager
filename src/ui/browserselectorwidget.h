#ifndef BROWSERSELECTORWIDGET_H
#define BROWSERSELECTORWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QVector>

namespace Ui {
class BrowserSelectorWidget;
}

class BrowserSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    enum BrowserCategory{ Download, Explore, Local };

    explicit BrowserSelectorWidget(QWidget *parent = nullptr);
    ~BrowserSelectorWidget();

    QTreeWidget *browserTreeWidget();
    QTreeWidgetItem *item(BrowserCategory category);
    QTreeWidgetItem *downloadItem();
    QTreeWidgetItem *exploreItem();
    QTreeWidgetItem *localItem();

signals:
    void browserChanged(BrowserCategory category, int index);
    void customContextMenuRequested(const QPoint &pos);

private slots:
    void addMultiple();
    void on_browserTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *);
    void on_addButton_clicked();
    void on_manageButton_clicked();
private:
    Ui::BrowserSelectorWidget *ui;

    QList<QTreeWidgetItem *> items_;
};

#endif // BROWSERSELECTORWIDGET_H
