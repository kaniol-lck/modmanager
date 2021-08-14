#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QList>

#include "moddirinfo.h"

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
    void on_modDirSelectorWidget_currentRowChanged(int currentRow);

private:
    Ui::ModManager *ui;
    QList<ModDirInfo> modDirList;
};
#endif // MODMANAGER_H
