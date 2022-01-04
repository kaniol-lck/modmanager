#include "pageswitcher.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenu>
#include <QLayout>
#include <QMenuBar>
#include <QDialog>

#include "ui/explorebrowser.h"
#include "ui/download/downloadbrowser.h"
#include "ui/curseforge/curseforgemodbrowser.h"
#include "ui/modrinth/modrinthmodbrowser.h"
#include "ui/optifine/optifinemodbrowser.h"
#include "ui/replay/replaymodbrowser.h"
#include "ui/github/githubrepobrowser.h"
#include "ui/local/localmodbrowser.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"

PageSwitcher::PageSwitcher(QWidget *parent) :
    QMdiArea(parent)
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
}

void PageSwitcher::previesPage()
{
    activatePreviousSubWindow();
}

void PageSwitcher::addDownloadPage()
{
    addBrowser(new DownloadBrowser(this), Download);
}

ExploreBrowser *PageSwitcher::exploreBrowser(int index) const
{
    return model_.item(Explore)->child(index)->data().value<ExploreBrowser *>();
}

void PageSwitcher::addExploreBrowser(ExploreBrowser *browser)
{
    addBrowser(browser, Explore);
}

void PageSwitcher::addLocalBrowser(LocalModBrowser *browser)
{
    browser->setPageSwitcher(this);
    addBrowser(browser, Local);
}

void PageSwitcher::removeExploreBrowser(int index)
{
    if(index < 0) return;
    removeSubWindowForItem(model_.item(Explore)->child(index));
    model_.item(Explore)->removeRow(index);
}

int PageSwitcher::findLocalBrowser(LocalModPath *path)
{
    for(int row = 0; row < model_.item(Local)->rowCount(); row++){
        auto item = model_.item(Local)->child(row);
        if(item->data().value<LocalModBrowser *>()->modPath() == path)
            return row;
    }
    return -1;
}

LocalModBrowser *PageSwitcher::localModBrowser(int index) const
{
    return model_.item(Local)->child(index)->data().value<LocalModBrowser *>();
}

void PageSwitcher::removeLocalBrowser(int index)
{
    auto item = model_.item(Local)->takeRow(index).first();
    item->data().value<LocalModBrowser *>()->deleteLater();
    removeSubWindowForItem(item);
}

void PageSwitcher::setPage(int category, int page)
{
    if(isSyncing_) return;
    auto previousBrowser = currentBrowser();
    auto item = model_.item(category)->child(page);
    auto window = item->data(SubWindowRole).value<QMdiSubWindow *>();
    if(!window) window = addSubWindowForItem(item);
//    if(window != currentSubWindow()){
    window->showMaximized();
    setActiveSubWindow(window);

    emit pageChanged(item->index());
    emit browserChanged(previousBrowser, currentBrowser());
    //    }
}

void PageSwitcher::syncPathList()
{
    isSyncing_ = true;
    //remember selected path
    LocalModBrowser *selectedBrowser = nullptr;
    auto [currentCategory, currentPage] = currentCategoryPage();
    if(currentCategory == PageSwitcher::Local)
        selectedBrowser = localModBrowser(currentPage);

    auto oldCount = model_.item(Local)->rowCount();
    for(const auto &path : LocalModPathManager::pathList()){
        if(auto i = findLocalBrowser(path); i < 0){
            //not present, new one
            addLocalBrowser(new LocalModBrowser(this, path));
        } else{
            //present, move position
            oldCount--;
            auto item = model_.item(Local)->takeRow(i).first();
            //update info
            item->setText(path->info().displayName());
            item->setIcon(path->info().icon());
            model_.item(Local)->appendRow(item);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        removeLocalBrowser(i);
    }

    //they should be same after sync
    isSyncing_ = false;

    //reset selected path
    if(selectedBrowser){
        if(auto index = findLocalBrowser(selectedBrowser->modPath()); index >= 0){
            setPage(PageSwitcher::Local, index);
            return;
        }
    }
    //fallback
    setPage(Explore, 0);
}

void PageSwitcher::updateUi()
{
    for(auto category : { Download, Explore, Local }){
        auto categoryItem = model_.item(category);
        for(int page = 0; page < categoryItem->rowCount(); page ++){
            if(auto browser = categoryItem->child(page)->data().value<Browser *>())
                browser->updateUi();
        }
    }
}

void PageSwitcher::addBrowser(Browser *browser, BrowserCategory category)
{
    auto item = new QStandardItem(browser->icon(), browser->name());
    item->setData(QVariant::fromValue(browser));
    Config config;
    if((category == Explore && config.getSearchModsOnStartup()) ||
            (category == Local && config.getLoadModsOnStartup()))
        addSubWindowForItem(item);
    model_.item(category)->appendRow(item);
}

QMdiSubWindow *PageSwitcher::addSubWindowForItem(QStandardItem *item)
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

void PageSwitcher::removeSubWindowForItem(QStandardItem *item)
{
    auto window = item->data(SubWindowRole).value<QMdiSubWindow *>();
    if(window){
        removeSubWindow(window);
        window->deleteLater();
        item->setData(QVariant(), SubWindowRole);
    }
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
    return model_.item(currentCategory)->child(currentPage)->data().value<Browser *>();
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
