#include "pageswitcher.h"

#include "ui/explorebrowser.h"
#include "ui/download/downloadbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/github/githubrepobrowser.h"
#include "ui/local/localmodbrowser.h"
#include "local/localmodpath.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenu>
#include <QLayout>
#include <QMenuBar>
#include <QDialog>

PageSwitcher::PageSwitcher(QWidget *parent) :
    QMdiArea(parent)/*,
    windows_(3)*/
{
    model_.appendRow(new QStandardItem(tr("Download")));
    model_.appendRow(new QStandardItem(tr("Explore")));
    model_.appendRow(new QStandardItem(tr("Local")));
    for(int row = 0; row < model_.rowCount(); row++){
        auto item = model_.item(row);
        item->setForeground(QColor(127, 127, 127));
        item->setFlags(item->flags().setFlag(Qt::ItemIsSelectable, false));
    };
//    setViewMode(QMdiArea::TabbedView);
}

void PageSwitcher::nextPage()
{
    activateNextSubWindow();
//    auto index = currentIndex() + 1;
//    if(index >= count())
//        index -= count();
//    setCurrentIndex(index);
}

void PageSwitcher::previesPage()
{
    activatePreviousSubWindow();
//    auto index = currentIndex() - 1;
//    if(index < 0)
//        index += count();
//    setCurrentIndex(index);
}

void PageSwitcher::addDownloadPage()
{
    if(!downloadBrowser_)
        downloadBrowser_ = new DownloadBrowser(this);
    addBrowser(downloadBrowser_, Download);
}

void PageSwitcher::addExploreBrowser(ExploreBrowser *browser)
{
    exploreBrowsers_ << browser;
    addBrowser(browser, Explore);
}

void PageSwitcher::addLocalBrowser(LocalModBrowser *browser)
{
    localModBrowsers_ << browser;
    addBrowser(browser, Local);
}

void PageSwitcher::removeExploreBrowser(int index)
{
    if(index < 0) return;
    removeSubWindow(exploreBrowsers_.at(index));
    exploreBrowsers_.removeAt(index);
    model_.item(Explore)->removeRow(index);
}

LocalModBrowser *PageSwitcher::takeLocalBrowser(int index)
{
    auto browser = localModBrowsers_.takeAt(index);
    removeSubWindow(browser);
    model_.item(Local)->removeRow(index);
    return browser;
}

void PageSwitcher::removeLocalBrowser(int index)
{
    takeLocalBrowser(index)->deleteLater();
}

void PageSwitcher::setPage(int category, int page)
{
    auto previousBrowser = currentBrowser();
    auto item = model_.item(category)->child(page);
    auto window = item->data(SubWindowRole).value<QMdiSubWindow *>();
    if(!window) window = addWidget(item);
//    if(window != currentSubWindow()){
    window->showMaximized();
    setActiveSubWindow(window);

//        emit pageChanged(model_.index(category, 0, model_.index(page, 0)));
    emit browserChanged(previousBrowser, currentBrowser());
//    }
}

void PageSwitcher::updateUi()
{
    for(auto browser : qAsConst(exploreBrowsers_))
        browser->updateUi();
    for(auto browser : qAsConst(localModBrowsers_))
        browser->updateUi();
}

void PageSwitcher::addBrowser(Browser *browser, BrowserCategory category)
{
    auto item = new QStandardItem(browser->icon(), browser->name());
    item->setData(QVariant::fromValue(browser));
    Config config;
    if((category == Explore && config.getSearchModsOnStartup()) ||
            (category == Local && config.getLoadModsOnStartup()))
        addWidget(item);
    model_.item(category)->appendRow(item);
}

QMdiSubWindow *PageSwitcher::addWidget(QStandardItem *item)
{
    auto browser = item->data().value<Browser *>();
    browser->load();
    //disable menubar
    browser->menuBar()->setNativeMenuBar(false);
    browser->menuBar()->hide();
    auto window = addSubWindow(browser, Qt::FramelessWindowHint);
    window->setWindowTitle(browser->name());
    window->setWindowIcon(browser->icon());
    item->setData(QVariant::fromValue(window), SubWindowRole);
    return window;
}

void PageSwitcher::removeExploreBrowser(ExploreBrowser *exploreBrowser)
{
    auto index = exploreBrowsers_.indexOf(exploreBrowser);
    if(index < 0) return;
    removeExploreBrowser(index);
}

QStandardItemModel *PageSwitcher::model()
{
    return &model_;
}

int PageSwitcher::currentCategory() const
{
    return currentCategoryPage().first;
}

int PageSwitcher::currentPage() const
{
    return currentCategoryPage().second;
}

Browser *PageSwitcher::currentBrowser() const
{
    auto [currentCategory, currentPage] = currentCategoryPage();
    switch (currentCategory) {
    case PageSwitcher::Download:
        return downloadBrowser_;
        break;
    case PageSwitcher::Explore:
        return exploreBrowsers_.at(currentPage);
        break;
    case PageSwitcher::Local:
        return localModBrowsers_.at(currentPage);
        break;
    }
    return nullptr;
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

QPair<int, int> PageSwitcher::currentCategoryPage() const
{
    auto window = currentSubWindow();
    for(auto category : { Download, Explore, Local }){
        auto categoryItem = model_.item(category);
        for(int page = 0; page < categoryItem->rowCount(); page ++){
            if(categoryItem->child(page)->data(SubWindowRole).value<QMdiSubWindow *>() == window)
                return { category, page };
        }
    }
    return { 1, 0 };
}
