#ifndef CURSEFORGEMODITEMWIDGET_H
#define CURSEFORGEMODITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;

namespace Ui {
class CurseforgeModItemWidget;
}

class CurseforgeModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod, const std::optional<CurseforgeFileInfo> &defaultDownload);
    ~CurseforgeModItemWidget();

private slots:
    void updateIcon();

    void on_downloadButton_clicked();

private:
    Ui::CurseforgeModItemWidget *ui;
    CurseforgeMod *curseforgeMod;
    std::optional<CurseforgeFileInfo> defaultFileInfo;
};

#endif // CURSEFORGEMODITEMWIDGET_H
