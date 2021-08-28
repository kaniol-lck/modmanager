#ifndef MODRINTHMODITEMWIDGET_H
#define MODRINTHMODITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthmod.h"

namespace Ui {
class ModrinthModItemWidget;
}

class ModrinthModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod);
    ~ModrinthModItemWidget();

private slots:
    void updateIcon();

private:
    Ui::ModrinthModItemWidget *ui;
    ModrinthMod *modrinthMod;
};

#endif // MODRINTHMODITEMWIDGET_H
