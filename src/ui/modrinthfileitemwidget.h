#ifndef MODRINTHFILEITEMWIDGET_H
#define MODRINTHFILEITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

namespace Ui {
class ModrinthFileItemWidget;
}

class ModrinthFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthFileItemWidget(QWidget *parent, const ModrinthFileInfo &info);
    ~ModrinthFileItemWidget();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::ModrinthFileItemWidget *ui;
    ModrinthFileInfo modrinthFileInfo;
};

#endif // MODRINTHFILEITEMWIDGET_H
