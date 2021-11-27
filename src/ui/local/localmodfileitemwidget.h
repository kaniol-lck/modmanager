#ifndef LOCALMODFILEITEMWIDGET_H
#define LOCALMODFILEITEMWIDGET_H

#include <QWidget>


class LocalModFile;
class QRadioButton;
namespace Ui {
class LocalModFileItemWidget;
}

class LocalModFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModFileItemWidget(QWidget *parent, LocalModFile *file);
    ~LocalModFileItemWidget();

    QRadioButton *button();
private slots:
    void onFileChanged();
private:
    Ui::LocalModFileItemWidget *ui;
    LocalModFile *file_;
};

#endif // LOCALMODFILEITEMWIDGET_H
