#ifndef REPLAYMODBROWSER_H
#define REPLAYMODBROWSER_H

#include "ui/explorebrowser.h"

#include "network/reply.hpp"
#include "replay/replaymodinfo.h"

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
    void searchModByPathInfo(LocalModPath *path) override;
    void updateUi() override;

    ExploreBrowser *another() override;

private slots:
    void filterList();
    void updateStatusText();

private:
    Ui::ReplayModBrowser *ui;
    ReplayAPI *api_;
    bool inited_ = false;
    std::unique_ptr<Reply<QList<ReplayModInfo> > > searchModsGetter_;

    void getModList();
    QWidget *getIndexWidget(QStandardItem *item) override;
};

#endif // REPLAYMODBROWSER_H
