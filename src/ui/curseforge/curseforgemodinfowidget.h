#ifndef CURSEFORGEMODINFOWIDGET_H
#define CURSEFORGEMODINFOWIDGET_H

#include <QWidget>

class CurseforgeMod;
namespace Ui {
class CurseforgeModInfoWidget;
}

class CurseforgeModInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModInfoWidget(QWidget *parent = nullptr);
    ~CurseforgeModInfoWidget();

    void setMod(CurseforgeMod *mod);
private:
    Ui::CurseforgeModInfoWidget *ui;
    CurseforgeMod *mod_ = nullptr;
    QList<QWidget*> tagWidgets_;
};

#endif // CURSEFORGEMODINFOWIDGET_H
