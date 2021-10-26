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
    explicit BrowserSelectorWidget(QWidget *parent);
    ~BrowserSelectorWidget();

    void setModel(QAbstractItemModel *model);
signals:
    void browserChanged(int category, int index);
    void customContextMenuRequested(QModelIndex index, QPoint pos);

private slots:
    void addMultiple();
    void onItemSelected(const QModelIndex &index);
    void on_addButton_clicked();
    void on_manageButton_clicked();
    void on_browserTreeView_customContextMenuRequested(const QPoint &pos);

private:
    Ui::BrowserSelectorWidget *ui;
};

#endif // BROWSERSELECTORWIDGET_H
