#ifndef OPTIFINEMODBROWSER_H
#define OPTIFINEMODBROWSER_H

#include "ui/explorebrowser.h"

#include "network/reply.hpp"
#include "optifine/optifinemodinfo.h"

class OptifineAPI;
class BMCLAPI;
class LocalModPath;
class ExploreStatusBarWidget;
class QStatusBar;
class OptifineManager;
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
    void searchModByPathInfo(LocalModPath *path) override;
    void updateUi() override;

    ExploreBrowser *another() override;

private slots:
    void updateStatusText();
    void on_actionGet_OptiFabric_triggered();
    void on_actionGet_OptiForge_triggered();

private:
    Ui::OptifineModBrowser *ui;
    OptifineManager *manager_;
    bool inited_ = false;

    QWidget *getIndexWidget(const QModelIndex &index) override;
};

#endif // OPTIFINEMODBROWSER_H
