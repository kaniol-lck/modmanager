#ifndef MODRINTHMODINFOWIDGET_H
#define MODRINTHMODINFOWIDGET_H

#include "ui/dockwidgetcontent.h"

class ModrinthMod;
namespace Ui {
class ModrinthModInfoWidget;
}

class ModrinthModInfoWidget : public DockWidgetContent
{
    Q_OBJECT

public:
    explicit ModrinthModInfoWidget(QWidget *parent = nullptr);
    ~ModrinthModInfoWidget();

    void setMod(ModrinthMod *mod);
signals:
    void modChanged();
private slots:
    void updateBasicInfo();
    void updateFullInfo();
    void updateIcon();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthModInfoWidget *ui;
    ModrinthMod *mod_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // MODRINTHMODINFOWIDGET_H
