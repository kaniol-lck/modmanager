#ifndef OPTIFINEMODBROWSER_H
#define OPTIFINEMODBROWSER_H

#include "ui/explorebrowser.h"

class OptifineAPI;
class BMCLAPI;
class LocalModPath;
class ExploreStatusBarWidget;
class QStatusBar;
class QStandardItemModel;
namespace Ui {
class OptifineModBrowser;
}

class OptifineModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit OptifineModBrowser(QWidget *parent = nullptr);
    ~OptifineModBrowser();

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
    void on_actionGet_OptiFabric_triggered();
    void on_actionGet_OptiForge_triggered();
    void on_actionOpen_Folder_triggered();

private:
    Ui::OptifineModBrowser *ui;
    OptifineAPI *api_;
    BMCLAPI *bmclapi_;
    LocalModPath *downloadPath_ = nullptr;
    bool inited_ = false;

    void getModList();
    QWidget *getIndexWidget(QStandardItem *item) override;
};

#endif // OPTIFINEMODBROWSER_H
