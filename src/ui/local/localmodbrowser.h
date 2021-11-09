#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include "ui/browser.h"
#include "local/localmodpathinfo.h"

class QListWidgetItem;
namespace Ui {
class LocalModBrowser;
}

class LocalModPath;
class LocalModFilter;
class QStatusBar;
class QProgressBar;
class LocalModInfoWidget;
class LocalModBrowser : public Browser
{
    Q_OBJECT

public:
    explicit LocalModBrowser(QWidget *parent, LocalModPath *modPath);
    ~LocalModBrowser();

    bool isLoading() const;
    LocalModPath *modPath() const;
    QWidget *infoWidget() const override;
signals:
    void loadFinished();

    void findNewOnCurseforge(LocalModPathInfo info);
    void findNewOnModrinth(LocalModPathInfo info);
    void findNewOnOptifine(LocalModPathInfo info);
    void findNewOnReplay(LocalModPathInfo info);

public slots:
    void reload();
    void updateModList();
    void updateUi();

private slots:
    void onLoadStarted();
    void onLoadProgress(int loadedCount, int totalCount);
    void onLoadFinished();

    void onCheckWebsitesStarted();
    void onWebsiteCheckedCountUpdated(int checkedCount);
    void onWebsitesReady();

    void onCheckUpdatesStarted();
    void onCheckedCountUpdated(int updateCount, int checkedCount);
    void onUpdatesReady();
    void onUpdatableCountChanged();

    void onUpdatesStarted();
    void onUpdatesProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdatesDoneCountUpdated(int doneCount, int totalCount);
    void onUpdatesDone(int successCount, int failCount);

    void filterList();
    void updateStatusText();

    void on_modListWidget_doubleClicked(const QModelIndex &index);
    void on_comboBox_currentIndexChanged(int index);
    void on_checkUpdatesButton_clicked();
    void on_openFolderButton_clicked();
    void on_checkButton_clicked();
    void on_updateAllButton_clicked();
    void on_hideUpdatesButton_clicked();

    void on_modListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::LocalModBrowser *ui;
    LocalModInfoWidget *infoWidget_;
    QStatusBar *statusBar_;
    QProgressBar *progressBar_;
    LocalModPath *modPath_;
    LocalModFilter *filter_;
    int hiddenCount_ = 0;
    bool isChecking_ = false;
    bool isUpdating_ = false;
};

#endif // LOCALMODBROWSER_H
