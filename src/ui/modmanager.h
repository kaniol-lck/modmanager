#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "local/localmodpathinfo.h"

class QListWidgetItem;
class LocalModPath;
class CurseforgeModBrowser;
class ModrinthModBrowser;

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
    void syncPathList();

    void editLocalPath(int index);

    void on_actionPreferences_triggered();

    void on_actionManage_Browser_triggered();

    void on_browserTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_browserTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int);

    void on_browserTreeWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModManager *ui;
    CurseforgeModBrowser *curseforgeModBrowser_;
    ModrinthModBrowser *modrinthModBrowser_;

    QList<LocalModPath*> pathList_;

    QTreeWidgetItem *downloadItem_;
    QTreeWidgetItem *exploreItem_;
    QTreeWidgetItem *localItem_;
};
#endif // MODMANAGER_H
