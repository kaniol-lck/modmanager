#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include <QWidget>

#include "local/localmodpathinfo.h"

namespace Ui {
class LocalModBrowser;
}

class LocalModPath;

class LocalModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModBrowser(QWidget *parent, LocalModPath *modPath);
    ~LocalModBrowser();

signals:
    void findNewOnCurseforge(LocalModPathInfo info);
    void findNewOnModrinth(LocalModPathInfo info);

public slots:
    void updateModList();

private slots:
    void startCheckWebsites();
    void websiteCheckedCountUpdated(int checkedCount);
    void websitesReady();

    void startCheckUpdates();
    void updateCheckedCountUpdated(int updateCount, int checkedCount);
    void updatesReady();

    void startUpdates();
    void updatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updatesDoneCountUpdated(int doneCount, int totalCount);
    void updatesDone(int successCount, int failCount);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_searchText_textEdited(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

    void on_checkUpdatesButton_clicked();

    void on_openFolderButton_clicked();

    void on_deleteOldButton_clicked();

    void on_checkButton_clicked();

    void on_updateAllButton_clicked();

    void on_modListWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::LocalModBrowser *ui;
    LocalModPath *modPath_;
    bool isUpdating_ = false;
};

#endif // LOCALMODBROWSER_H
