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
    void refreshBrowsers();

    void on_modDirSelectorWidget_currentRowChanged(int currentRow);

    void on_newLocalBrowserButton_clicked();

    void on_modDirSelectorWidget_doubleClicked(const QModelIndex &index);

    void on_actionPreferences_triggered();

    void on_actionManage_Browser_triggered();

private:
    Ui::ModManager *ui;
    QList<ModDirInfo> modDirList;
    QList<QListWidgetItem*> dirWidgetItemList;
    QList<LocalModBrowser*> localModBrowserList;
    QFutureWatcher<void> *updateVersionsWatcher;
    int specialBrowserCount = 0;
};
#endif // MODMANAGER_H
