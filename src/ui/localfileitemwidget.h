#ifndef LOCALFILEITEMWIDGET_H
#define LOCALFILEITEMWIDGET_H

#include <QWidget>

#include "local/localmodinfo.h"

class LocalMod;

namespace Ui {
class LocalFileItemWidget;
}

class LocalFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalFileItemWidget(QWidget *parent, LocalMod *mod, const LocalModInfo &info);
    ~LocalFileItemWidget();

private slots:
    void on_rollbackButton_clicked();

    void on_openFolderButton_clicked();

private:
    Ui::LocalFileItemWidget *ui;
    LocalMod *mod_;
    LocalModInfo info_;
};

#endif // LOCALFILEITEMWIDGET_H
