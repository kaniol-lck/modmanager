#ifndef REPLAYMODBROWSER_H
#define REPLAYMODBROWSER_H

#include "ui/explorebrowser.h"

class LocalModPath;
class ReplayAPI;
class ExploreStatusBarWidget;
class QStatusBar;
class QStandardItemModel;
namespace Ui {
class ReplayModBrowser;
}

class ReplayModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit ReplayModBrowser(QWidget *parent = nullptr);
    ~ReplayModBrowser();

    void load() override;

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

    ExploreBrowser *another() override;

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
    void updateLocalPathList();
    void filterList();
    void updateStatusText();
    void on_downloadPathSelect_currentIndexChanged(int index);
    void on_actionOpen_Folder_triggered();

private:
    Ui::ReplayModBrowser *ui;
    ReplayAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    bool inited_ = false;

    void getModList();
    QWidget *getIndexWidget(QStandardItem *item) override;
};

#endif // REPLAYMODBROWSER_H
