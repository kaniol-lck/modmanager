#ifndef LOCALFILEITEMWIDGET_H
#define LOCALFILEITEMWIDGET_H

#include <QWidget>

class LocalModFile;
class LocalMod;

namespace Ui {
class LocalFileItemWidget;
}

class LocalFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalFileItemWidget(QWidget *parent, LocalMod *mod, LocalModFile *file);
    ~LocalFileItemWidget();

private slots:
    void updateFileInfo();

    void on_rollbackButton_clicked();

    void on_openFolderButton_clicked();

private:
    Ui::LocalFileItemWidget *ui;
    LocalMod *mod_;
    LocalModFile *file_;
};

#endif // LOCALFILEITEMWIDGET_H
