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
    void updateStatusText();

private:
    Ui::ReplayModBrowser *ui;
    ReplayManager *manager_;
    bool inited_ = false;
    std::unique_ptr<Reply<QList<ReplayModInfo> > > searchModsGetter_;

    QWidget *getIndexWidget(const QModelIndex &index) override;
};

#endif // REPLAYMODBROWSER_H
