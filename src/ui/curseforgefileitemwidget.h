#ifndef CURSEFORGEFILEITEMWIDGET_H
#define CURSEFORGEFILEITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

namespace Ui {
class CurseforgeFileItemWidget;
}

class CurseforgeFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileItemWidget(QWidget *parent, const CurseforgeFileInfo &info);
    ~CurseforgeFileItemWidget();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::CurseforgeFileItemWidget *ui;
    CurseforgeFileInfo fileInfo_;
};

#endif // CURSEFORGEFILEITEMWIDGET_H
