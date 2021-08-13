#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QList>
#include "modinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ModManager; }
QT_END_NAMESPACE

class ModManager : public QMainWindow
{
    Q_OBJECT

public:
    ModManager(QWidget *parent = nullptr);
    ~ModManager();

private slots:
    void on_modListWidget_currentRowChanged(int currentRow);

private:
    Ui::ModManager *ui;
    QList<ModInfo> modList;
};
#endif // MODMANAGER_H
