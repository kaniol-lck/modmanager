#include "pageswitcher.h"

#include "ui/explorebrowser.h"
#include "ui/download/downloadbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/local/localmodbrowser.h"

#include <QDebug>

PageSwitcher::PageSwitcher(QWidget *parent) :
    QStackedWidget(parent),
    pageCount_(3)
{}

void PageSwitcher::addDownloadPage()
{
    downloadBrowser_ = new DownloadBrowser(this);
    addWidget(downloadBrowser_);
    pageCount_[Download]++;
}

void PageSwitcher::addCurseforgePage()
{
    curseforgeModBrowser_ = new CurseforgeModBrowser(this);
    exploreBrowsers_ << curseforgeModBrowser_;
    addWidget(curseforgeModBrowser_);
    pageCount_[Explore]++;
}

void PageSwitcher::addModrinthPage()
{
    modrinthModBrowser_ = new ModrinthModBrowser(this);
    exploreBrowsers_ << modrinthModBrowser_;
    addWidget(modrinthModBrowser_);
    pageCount_[Explore]++;
}

void PageSwitcher::addOptiFinePage()
{
    optifineModBrowser_ = new OptifineModBrowser(this);
    exploreBrowsers_ << optifineModBrowser_;
    addWidget(optifineModBrowser_);
    pageCount_[Explore]++;
}

void PageSwitcher::addReplayModPage()
{
    replayModBrowser_ = new ReplayModBrowser(this);
    exploreBrowsers_ << replayModBrowser_;
    addWidget(replayModBrowser_);
    pageCount_[Explore]++;
}

void PageSwitcher::addLocalPage(LocalModBrowser *browser)
{
    localModBrowsers_ << browser;
    addWidget(browser);
    pageCount_[Local]++;

    connect(browser, &LocalModBrowser::findNewOnCurseforge, curseforgeModBrowser_, &CurseforgeModBrowser::searchModByPathInfo);
    connect(browser, &LocalModBrowser::findNewOnCurseforge, this, [=]{
        setPage(Explore, exploreBrowsers_.indexOf(curseforgeModBrowser_));
    });
    connect(browser, &LocalModBrowser::findNewOnModrinth, modrinthModBrowser_, &ModrinthModBrowser::searchModByPathInfo);
    connect(browser, &LocalModBrowser::findNewOnModrinth, this, [=]{
        setPage(Explore, exploreBrowsers_.indexOf(modrinthModBrowser_));
    });
    connect(browser, &LocalModBrowser::findNewOnOptifine, optifineModBrowser_, &OptifineModBrowser::searchModByPathInfo);
    connect(browser, &LocalModBrowser::findNewOnOptifine, this, [=]{
        setPage(Explore, exploreBrowsers_.indexOf(optifineModBrowser_));
    });
    connect(browser, &LocalModBrowser::findNewOnReplay, replayModBrowser_, &ReplayModBrowser::searchModByPathInfo);
    connect(browser, &LocalModBrowser::findNewOnReplay, this, [=]{
        setPage(Explore, exploreBrowsers_.indexOf(replayModBrowser_));
    });
}

void PageSwitcher::addLocalPage(LocalModPath *path)
{
    auto localModBrowser = new LocalModBrowser(this, path);
    addLocalPage(localModBrowser);
}

LocalModBrowser *PageSwitcher::takeLocalModBrowser(int index)
{
    auto browser = localModBrowsers_.takeAt(index);
    removeWidget(browser);
    pageCount_[Local]--;
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
