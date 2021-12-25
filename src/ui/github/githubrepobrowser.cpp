#include "githubrepobrowser.h"
#include "ui_githubrepobrowser.h"

#include <QStandardItemModel>
#include <QUrl>

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
    infoWidget_(new GitHubReleaseInfoWidget(this)),
    fileListWidget_(new GitHubFileListWidget(this)),
    api_(new GitHubAPI())
{
    ui->setupUi(this);
    initUi();

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

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

}

void GitHubRepoBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{

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
    auto str = tr("Loaded %1 releases from GitHub.").arg(model_->rowCount());
    ui->statusbar->showMessage(str);
}

void GitHubRepoBrowser::getReleaseList(int page)
{
    if(page == 1)
        currentPage_ = 1;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    isSearching_ = true;
    auto conn = api_->getReleases(info_, page, [=](const auto &list){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);
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
        isSearching_ = false;

        updateStatusText();
    });

}

void GitHubRepoBrowser::loadMore()
{
    if(!isSearching_ && hasMore_){
        currentPage_ ++;
        getReleaseList(currentPage_);
    }
}

void GitHubRepoBrowser::onSelectedItemChanged(QStandardItem *item)
{
    if(item){
        auto release = item->data().value<GitHubRelease*>();
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

QWidget *GitHubRepoBrowser::getIndexWidget(QStandardItem *item)
{
    auto release = item->data().value<GitHubRelease*>();
    if(release){
        auto widget = new GitHubReleaseItemWidget(this, release->info());
//            modItemWidget->setDownloadPath(downloadPath_);
//            connect(this, &ReplayModBrowser::downloadPathChanged, modItemWidget, &ReplayModItemWidget::setDownloadPath);
        return widget;
    }
}
