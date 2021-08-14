#ifndef CURSEFORGEMODITEMWIDGET_H
#define CURSEFORGEMODITEMWIDGET_H

#include <QWidget>

class CurseforgeMod;

namespace Ui {
class CurseforgeModItemWidget;
}

class CurseforgeModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod);
    ~CurseforgeModItemWidget();

private slots:
    void updateThumbnail();

private:
    Ui::CurseforgeModItemWidget *ui;
    CurseforgeMod *curseforgeMod;
};

#endif // CURSEFORGEMODITEMWIDGET_H
