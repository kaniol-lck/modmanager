#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QList>
#include <QFutureWatcher>

#include "moddirinfo.h"

class QListWidgetItem;
class LocalModBrowser;

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

    void on_newLocalBrowserButton_clicked();

    void on_modDirSelectorWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::ModManager *ui;
    QList<ModDirInfo> modDirList;
    QList<QListWidgetItem*> dirWidgetItemList;
    QList<LocalModBrowser*> localModBrowserList;
    QFutureWatcher<void> *updateVersionsWatcher;
};
#endif // MODMANAGER_H
