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

private:
    Ui::LocalModBrowser *ui;
    LocalModPath *modPath_;

    enum CheckUpdateStatus{
        SearchOnWebsites,
        CheckUpdates,
        ReadyUpdate,
        Updating,
        UpdateDone
    } status_ = SearchOnWebsites;

};

#endif // LOCALMODBROWSER_H
