#ifndef CURSEFORGEMODINFOWIDGET_H
#define CURSEFORGEMODINFOWIDGET_H

#include "ui/dockwidgetcontent.h"

class CurseforgeMod;
namespace Ui {
class CurseforgeModInfoWidget;
}

class CurseforgeModInfoWidget : public DockWidgetContent
{
    Q_OBJECT

public:
    explicit CurseforgeModInfoWidget(QWidget *parent = nullptr);
    ~CurseforgeModInfoWidget();

    void setMod(CurseforgeMod *mod);
signals:
    void modChanged();
private slots:
    void updateBasicInfo();
    void updateThumbnail();
    void updateDescription();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CurseforgeModInfoWidget *ui;
    CurseforgeMod *mod_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // CURSEFORGEMODINFOWIDGET_H
