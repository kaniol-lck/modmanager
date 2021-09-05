#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QTreeWidgetItem>
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

    void on_actionPreferences_triggered();

    void on_actionManage_Browser_triggered();

    void on_browserTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_browserTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int);

private:
    Ui::ModManager *ui;
    QList<ModDirInfo> modDirList_;
    QList<QTreeWidgetItem*> dirWidgetItemList_;
    QList<LocalModBrowser*> localModBrowserList_;
    QFutureWatcher<void> *updateVersionsWatcher_;

    QTreeWidgetItem *downloadItem_;
    QTreeWidgetItem *exploreItem_;
    QTreeWidgetItem *localItem_;
};
#endif // MODMANAGER_H
