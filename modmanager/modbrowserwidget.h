#ifndef MODBROWSERWIDGET_H
#define MODBROWSERWIDGET_H

#include <QWidget>
#include <QDir>

#include "modinfo.h"

namespace Ui {
class ModBrowserWidget;
}

class ModBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModBrowserWidget(QWidget *parent, QDir path);
    ~ModBrowserWidget();

private slots:
    void on_modListWidget_currentRowChanged(int currentRow);

private:
    Ui::ModBrowserWidget *ui;
    QDir modFolderPath;
    QList<ModInfo> modList;
};

#endif // MODBROWSERWIDGET_H
