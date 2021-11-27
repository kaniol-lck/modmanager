#ifndef LOCALMODFILEITEMWIDGET_H
#define LOCALMODFILEITEMWIDGET_H

#include <QWidget>


class LocalModFile;
namespace Ui {
class LocalModFileItemWidget;
}

class LocalModFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModFileItemWidget(QWidget *parent, LocalModFile *file);
    ~LocalModFileItemWidget();
private slots:
    void onFileChanged();
private:
    Ui::LocalModFileItemWidget *ui;
    LocalModFile *file_;
};

#endif // LOCALMODFILEITEMWIDGET_H
