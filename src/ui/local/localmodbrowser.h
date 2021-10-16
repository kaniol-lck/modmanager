#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include <QWidget>

#include "local/localmodpathinfo.h"

namespace Ui {
class LocalModBrowser;
}

class LocalModPath;
class LocalModFilter;

class LocalModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModBrowser(QWidget *parent, LocalModPath *modPath);
    ~LocalModBrowser();

signals:
    void findNewOnCurseforge(LocalModPathInfo info);
    void findNewOnModrinth(LocalModPathInfo info);
    void findNewOnOptifine(LocalModPathInfo info);
    void findNewOnReplay(LocalModPathInfo info);

public slots:
    void updateModList();

private slots:
    void loadStarted();
    void loadProgress(int loadedCount, int totalCount);
    void loadFinished();

    void startCheckWebsites();
    void websiteCheckedCountUpdated(int checkedCount);
    void websitesReady();

    void startCheckUpdates();
    void updateCheckedCountUpdated(int updateCount, int checkedCount);
    void updateUpdatableCount(int count);
    void updatesReady();

    void startUpdates();
    void updatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updatesDoneCountUpdated(int doneCount, int totalCount);
    void updatesDone(int successCount, int failCount);

    void filterList();

    void on_modListWidget_doubleClicked(const QModelIndex &index);
    void on_comboBox_currentIndexChanged(int index);
    void on_checkUpdatesButton_clicked();
    void on_openFolderButton_clicked();
    void on_checkButton_clicked();
    void on_updateAllButton_clicked();
private:
    Ui::LocalModBrowser *ui;
    LocalModPath *modPath_;
    LocalModFilter *filter_;
    bool isChecking_ = false;
    bool isUpdating_ = false;
};

#endif // LOCALMODBROWSER_H
