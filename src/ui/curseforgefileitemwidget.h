#ifndef CURSEFORGEFILEITEMWIDGET_H
#define CURSEFORGEFILEITEMWIDGET_H

#include <QWidget>
#include <QTimer>

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

    void updateDownlaodSpeed();

private:
    Ui::CurseforgeFileItemWidget *ui;
    CurseforgeFileInfo curseforgeFileInfo;
    QTimer speedTimer;
    qint64 lastDownloadBytes = 0;
};

#endif // CURSEFORGEFILEITEMWIDGET_H
