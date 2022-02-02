#include "githubrepobrowser.h"
#include "ui_githubrepobrowser.h"

#include <QStandardItemModel>
#include <QUrl>

#include "github/githubmanager.h"
#include "local/localmodpath.h"
#include "ui/downloadpathselectmenu.h"
#include "githubreleaseinfowidget.h"
#include "githubfilelistwidget.h"
#include "github/githubrelease.h"
#include "githubreleaseitemwidget.h"
#include "github/githubapi.h"
#include "ui/explorestatusbarwidget.h"
#include "util/smoothscrollbar.h"

GitHubRepoBrowser::GitHubRepoBrowser(QWidget *parent, const GitHubRepoInfo &info) :
    ExploreBrowser(parent, info.icon(), info.name() + " (GitHub)", info.url()),
    info_(info),
    ui(new Ui::GitHubRepoBrowser),
    manager_(new GitHubManager(this, info)),
    infoWidget_(new GitHubReleaseInfoWidget(this)),
    fileListWidget_(new GitHubFileListWidget(this))
{
    ui->setupUi(this);
    ui->menu_GitHub->addActions(menu_->actions());
    initUi(manager_);

    ui->toolBar->addMenu(downloadPathSelectMenu_);
    ui->toolBar->addAction(refreshAction_);
    ui->toolBar->addAction(visitWebsiteAction_);
    ui->toolBar->addAction(openDialogAction_);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        refresh();
    }
}

GitHubRepoBrowser::~GitHubRepoBrowser()
{
    delete ui;
}

void GitHubRepoBrowser::load()
{
    if(!inited_){
        inited_ = true;
        refresh();
    }
}

QWidget *GitHubRepoBrowser::infoWidget() const
{
    return infoWidget_;
}

QWidget *GitHubRepoBrowser::fileListWidget() const
{
    return fileListWidget_;
}

void GitHubRepoBrowser::refresh()
{
    manager_->search();
}

void GitHubRepoBrowser::searchModByPathInfo(LocalModPath *path)
{
    downloadPathSelectMenu_->setDownloadPath(path);
}

void GitHubRepoBrowser::updateUi()
{

}

ExploreBrowser *GitHubRepoBrowser::another()
{
    return new GitHubRepoBrowser(nullptr, info_);
}

void GitHubRepoBrowser::loadMore()
{
    if(refreshAction_->isEnabled())
        manager_->searchMore();
}

void GitHubRepoBrowser::onSelectedItemChanged(const QModelIndex &index)
{
    if(index.isValid()){
        auto release = index.data(Qt::UserRole + 1).value<GitHubRelease*>();
        if(release){
            infoWidget_->setRelease(release);
            fileListWidget_->setRelease(release);
            if(release != selectedRelease_){
                selectedRelease_ = release;
            }
        } else
            selectedRelease_ = nullptr;
    } else
        selectedRelease_ = nullptr;
}

QWidget *GitHubRepoBrowser::getIndexWidget(const QModelIndex &index)
{
    auto release = index.data(Qt::UserRole + 1).value<GitHubRelease*>();
    if(release){
        auto widget = new GitHubReleaseItemWidget(this, release->info());
        manager_->model()->setItemHeight(widget->height());
        return widget;
    } else
        return nullptr;
}
