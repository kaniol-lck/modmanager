#ifndef REPLAYMODBROWSER_H
#define REPLAYMODBROWSER_H

#include "ui/explorebrowser.h"

#include "network/reply.hpp"
#include "replay/replaymodinfo.h"

class LocalModPath;
class ReplayAPI;
class ExploreStatusBarWidget;
class QStatusBar;
class ReplayManager;

class ReplayManagerProxyModel;
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
    void on_versionSelect_currentIndexChanged(int index);
    void on_loaderSelect_currentIndexChanged(int index);
    void on_searchText_textChanged(const QString &arg1);

private:
    Ui::ReplayModBrowser *ui;
    ReplayManager *manager_;
    ReplayManagerProxyModel *proxyModel_;
    QList<GameVersion> gameVersions_;
    bool inited_ = false;
    std::unique_ptr<Reply<QList<ReplayModInfo> > > searchModsGetter_;

    QWidget *getListViewIndexWidget(const QModelIndex &index) override;
};

#endif // REPLAYMODBROWSER_H
