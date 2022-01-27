#include "githubrepobrowser.h"
#include "ui_githubrepobrowser.h"

#include <QStandardItemModel>
#include <QUrl>

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
    model_(new QStandardItemModel(this)),
    infoWidget_(new GitHubReleaseInfoWidget(this)),
    fileListWidget_(new GitHubFileListWidget(this)),
    api_(new GitHubAPI())
{
    ui->setupUi(this);
    ui->menu_GitHub->addActions(menu_->actions());
    initUi(model_);

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);
    ui->toolBar->addMenu(downloadPathSelectMenu_);
    ui->toolBar->addAction(refreshAction_);
    ui->toolBar->addAction(visitWebsiteAction_);
    ui->toolBar->addAction(openDialogAction_);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getReleaseList();
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
        getReleaseList();
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
    getReleaseList();
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

void GitHubRepoBrowser::updateStatusText()
{
    statusBarWidget_->setModCount(model_->rowCount() - (hasMore_? 0 : 1));
}

void GitHubRepoBrowser::getReleaseList(int page)
{
    if(!refreshAction_->isEnabled()) return;
    if(page == 1)
        currentPage_ = 1;
    else if(!hasMore_)
        return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);
    refreshAction_->setEnabled(false);
    searchModsGetter_ = api_->getReleases(info_, page).asUnique();
    searchModsGetter_->setOnFinished(this, [=](const auto &list){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
        //new search
        if(currentPage_ == 1){
            model_->clear();
            hasMore_ = true;
        }
        for(auto releaseInfo : list){
            auto release = new GitHubRelease(this, releaseInfo);
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(release));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 200));
        }
        if(list.size() < Config().getSearchResultCount()){
            auto item = new QStandardItem(tr("There is no more mod here..."));
            item->setSizeHint(QSize(0, 108));
            auto font = qApp->font();
            font.setPointSize(20);
            item->setFont(font);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            model_->appendRow(item);
            hasMore_ = false;
        }
        updateStatusText();
    }, [=](auto){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText(tr("Failed loading"));
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
    });
}

void GitHubRepoBrowser::loadMore()
{
    if(refreshAction_->isEnabled() && hasMore_){
        currentPage_ ++;
        getReleaseList(currentPage_);
    }
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
        return widget;
    } else
        return nullptr;
}
