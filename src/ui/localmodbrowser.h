#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include <QWidget>

#include "local/localmod.h"

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

public slots:
    void updateModList();

private slots:
    void startCheckWebsites();
    void websiteCheckedCountUpdated(int checkedCount);
    void websitesReady();
    void startCheckUpdates();
    void updateCheckedCountUpdated(int updateCount, int checkedCount);
    void updatesReady(int updateCount);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_searchText_textEdited(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

    void on_checkUpdatesButton_clicked();

private:
    Ui::LocalModBrowser *ui;
    LocalModPath *modPath_;

    enum CheckUpdateStatus{
        SearchOnWebsites,
        CheckUpdates,
        ReadyUpdate
    } status_ = SearchOnWebsites;

};

#endif // LOCALMODBROWSER_H
