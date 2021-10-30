#include "pageswitcher.h"

#include "ui/explorebrowser.h"
#include "ui/download/downloadbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/local/localmodbrowser.h"
#include "local/localmodpath.h"

#include <QDebug>

PageSwitcher::PageSwitcher(QWidget *parent) :
    QStackedWidget(parent),
    pageCount_(3)
{
    model_.appendRow(new QStandardItem(tr("Download")));
    model_.appendRow(new QStandardItem(tr("Explore")));
    model_.appendRow(new QStandardItem(tr("Local")));
    for(int row = 0; row < model_.rowCount(); row++){
        auto item = model_.item(row);
        item->setForeground(QColor(127, 127, 127));
        item->setFlags(item->flags().setFlag(Qt::ItemIsSelectable, false));
    };
}

void PageSwitcher::nextPage()
{
    auto index = currentIndex() + 1;
    if(index >= count())
        index -= count();
    setCurrentIndex(index);
}

void PageSwitcher::previesPage()
{
    auto index = currentIndex() - 1;
    if(index < 0)
        index += count();
    setCurrentIndex(index);
}

void PageSwitcher::addDownloadPage()
{
    if(!downloadBrowser_)
        downloadBrowser_ = new DownloadBrowser(this);
    addWidget(downloadBrowser_);
    pageCount_[Download]++;
    auto item = new QStandardItem(QIcon::fromTheme("download"), tr("Downloader"));
    model_.item(Download)->appendRow(item);
}

void PageSwitcher::addCurseforgePage()
{
    if(!curseforgeModBrowser_)
        curseforgeModBrowser_ = new CurseforgeModBrowser(this);
    exploreBrowsers_ << curseforgeModBrowser_;
    insertWidget(pageCount_[Download] + pageCount_[Explore], curseforgeModBrowser_);
    pageCount_[Explore]++;
    auto item = new QStandardItem(QIcon(":/image/curseforge.svg"), tr("Curseforge"));
    model_.item(Explore)->appendRow(item);
}

void PageSwitcher::addModrinthPage()
{
    if(!modrinthModBrowser_)
        modrinthModBrowser_ = new ModrinthModBrowser(this);
    exploreBrowsers_ << modrinthModBrowser_;
    insertWidget(pageCount_[Download] + pageCount_[Explore], modrinthModBrowser_);
    pageCount_[Explore]++;
    auto item = new QStandardItem(QIcon(":/image/modrinth.svg"), tr("Modrinth"));
    model_.item(Explore)->appendRow(item);
}

void PageSwitcher::addOptiFinePage()
{
    if(!optifineModBrowser_)
        optifineModBrowser_ = new OptifineModBrowser(this);
    exploreBrowsers_ << optifineModBrowser_;
    insertWidget(pageCount_[Download] + pageCount_[Explore], optifineModBrowser_);
    pageCount_[Explore]++;
    auto item = new QStandardItem(QIcon(":/image/optifine.png"), tr("OptiFine"));
    model_.item(Explore)->appendRow(item);
}

void PageSwitcher::addReplayModPage()
{
    if(!replayModBrowser_)
        replayModBrowser_ = new ReplayModBrowser(this);
    exploreBrowsers_ << replayModBrowser_;
    insertWidget(pageCount_[Download] + pageCount_[Explore], replayModBrowser_);
    pageCount_[Explore]++;
    auto item = new QStandardItem(QIcon(":/image/replay.png"), tr("ReplayMod"));
    model_.item(Explore)->appendRow(item);
}

void PageSwitcher::addLocalPage(LocalModBrowser *browser)
{
    localModBrowsers_ << browser;
    addWidget(browser);
    pageCount_[Local]++;
    auto item = new QStandardItem(browser->modPath()->info().icon(), browser->modPath()->info().displayName());
    model_.item(Local)->appendRow(item);

    if(curseforgeModBrowser_){
        connect(browser, &LocalModBrowser::findNewOnCurseforge, curseforgeModBrowser_, &CurseforgeModBrowser::searchModByPathInfo);
        connect(browser, &LocalModBrowser::findNewOnCurseforge, this, [=]{
            setPage(Explore, exploreBrowsers_.indexOf(curseforgeModBrowser_));
        });
    }
    if(modrinthModBrowser_){
        connect(browser, &LocalModBrowser::findNewOnModrinth, modrinthModBrowser_, &ModrinthModBrowser::searchModByPathInfo);
        connect(browser, &LocalModBrowser::findNewOnModrinth, this, [=]{
            setPage(Explore, exploreBrowsers_.indexOf(modrinthModBrowser_));
        });
    }
    if(optifineModBrowser_){
        connect(browser, &LocalModBrowser::findNewOnOptifine, optifineModBrowser_, &OptifineModBrowser::searchModByPathInfo);
        connect(browser, &LocalModBrowser::findNewOnOptifine, this, [=]{
            setPage(Explore, exploreBrowsers_.indexOf(optifineModBrowser_));
        });
    }
    if(replayModBrowser_){
        connect(browser, &LocalModBrowser::findNewOnReplay, replayModBrowser_, &ReplayModBrowser::searchModByPathInfo);
        connect(browser, &LocalModBrowser::findNewOnReplay, this, [=]{
            setPage(Explore, exploreBrowsers_.indexOf(replayModBrowser_));
        });
    }
}

void PageSwitcher::addLocalPage(LocalModPath *path)
{
    auto localModBrowser = new LocalModBrowser(this, path);
    addLocalPage(localModBrowser);
}

void PageSwitcher::removeExplorePage(int index)
{
    if(index < 0) return;
    removeWidget(exploreBrowsers_.at(index));
    exploreBrowsers_.removeAt(index);
    pageCount_[Explore]--;
    model_.item(Explore)->removeRow(index);
}

void PageSwitcher::removeCurseforgePage()
{
    removeExplorePage(curseforgeModBrowser_);
}

void PageSwitcher::removeModrinthPage()
{
    removeExplorePage(modrinthModBrowser_);
}

void PageSwitcher::removeOptiFinePage()
{
    removeExplorePage(optifineModBrowser_);
}

void PageSwitcher::removeReplayModPage()
{
    removeExplorePage(replayModBrowser_);
}

LocalModBrowser *PageSwitcher::takeLocalModBrowser(int index)
{
    auto browser = localModBrowsers_.takeAt(index);
    removeWidget(browser);
    pageCount_[Local]--;
    model_.item(Local)->removeRow(index);
    return browser;
}

void PageSwitcher::removeLocalModBrowser(int index)
{
    takeLocalModBrowser(index)->deleteLater();
}

void PageSwitcher::setPage(int category, int page)
{
    auto index = std::accumulate(pageCount_.cbegin(), pageCount_.cbegin() + category, 0) + page;
    currentCategory_ = category;
    currentPage_ = page;
    setCurrentIndex(index);
}

void PageSwitcher::updateUi()
{
    for(auto browser : qAsConst(exploreBrowsers_))
        browser->updateUi();
    for(auto browser : qAsConst(localModBrowsers_))
        browser->updateUi();
    Config config;
    if(config.getShowCurseforge() && !exploreBrowsers_.contains(curseforgeModBrowser_)) addCurseforgePage();
    if(!config.getShowCurseforge() && exploreBrowsers_.contains(curseforgeModBrowser_)) removeCurseforgePage();
    if(config.getShowModrinth() && !exploreBrowsers_.contains(modrinthModBrowser_)) addModrinthPage();
    if(!config.getShowModrinth() && exploreBrowsers_.contains(modrinthModBrowser_)) removeModrinthPage();
    if(config.getShowOptiFine() && !exploreBrowsers_.contains(optifineModBrowser_)) addOptiFinePage();
    if(!config.getShowOptiFine() && exploreBrowsers_.contains(optifineModBrowser_)) removeOptiFinePage();
    if(config.getShowReplayMod() && !exploreBrowsers_.contains(replayModBrowser_)) addReplayModPage();
    if(!config.getShowReplayMod() && exploreBrowsers_.contains(replayModBrowser_)) removeReplayModPage();
}

void PageSwitcher::removeExplorePage(ExploreBrowser *exploreBrowser)
{
    auto index = exploreBrowsers_.indexOf(exploreBrowser);
    if(index < 0) return;
    removeExplorePage(index);
}

QStandardItemModel *PageSwitcher::model()
{
    return &model_;
}

int PageSwitcher::currentCategory() const
{
    return currentCategory_;
}

int PageSwitcher::currentPage() const
{
    return currentPage_;
}

const QList<ExploreBrowser *> &PageSwitcher::exploreBrowsers() const
{
    return exploreBrowsers_;
}

const QList<LocalModBrowser *> &PageSwitcher::localModBrowsers() const
{
    return localModBrowsers_;
}

DownloadBrowser *PageSwitcher::downloadBrowser() const
{
    return downloadBrowser_;
}

CurseforgeModBrowser *PageSwitcher::curseforgeModBrowser() const
{
    return curseforgeModBrowser_;
}

ModrinthModBrowser *PageSwitcher::modrinthModBrowser() const
{
    return modrinthModBrowser_;
}

OptifineModBrowser *PageSwitcher::optifineModBrowser() const
{
    return optifineModBrowser_;
}

ReplayModBrowser *PageSwitcher::replayModBrowser() const
{
    return replayModBrowser_;
}

ExploreBrowser *PageSwitcher::exploreBrowser(int index) const
{
    if(index >= exploreBrowsers_.size())
        return nullptr;
    return exploreBrowsers_.at(index);
}

LocalModBrowser *PageSwitcher::localModBrowser(int index) const
{
    if(index >= localModBrowsers_.size())
        return nullptr;
    return localModBrowsers_.at(index);
}
