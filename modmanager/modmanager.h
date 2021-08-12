#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ModManager; }
QT_END_NAMESPACE

class ModManager : public QMainWindow
{
    Q_OBJECT

public:
    ModManager(QWidget *parent = nullptr);
    ~ModManager();

private:
    Ui::ModManager *ui;
};
#endif // MODMANAGER_H
