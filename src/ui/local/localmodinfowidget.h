#ifndef LOCALMODINFOWIDGET_H
#define LOCALMODINFOWIDGET_H

#include <QWidget>

class LocalMod;
namespace Ui {
class LocalModInfoWidget;
}

class LocalModInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModInfoWidget(QWidget *parent = nullptr);
    ~LocalModInfoWidget();

    void setMod(LocalMod *mod);
private slots:
    void updateInfo();
private:
    Ui::LocalModInfoWidget *ui;
    LocalMod *mod_;
    QList<QWidget*> tagWidgets_;
};

#endif // LOCALMODINFOWIDGET_H
