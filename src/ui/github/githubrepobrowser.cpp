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
    model_(new QStandardItemModel(this)),
    infoWidget_(new GitHubReleaseInfoWidget(this)),
    fileListWidget_(new GitHubFileListWidget(this)),
    statusBarWidget_(new ExploreStatusBarWidget(this)),
    api_(new GitHubAPI())
{
    ui->setupUi(this);
    ui->releaseListView->setModel(model_);
    ui->releaseListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->releaseListView->setProperty("class", "ModList");

    setCentralWidget(ui->releaseListView);

    //setup status bar
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    connect(ui->releaseListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &GitHubRepoBrowser::onSliderChanged);
    connect(ui->releaseListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &GitHubRepoBrowser::onItemSelected);
    connect(ui->releaseListView->verticalScrollBar(), &QScrollBar::valueChanged, this, &GitHubRepoBrowser::updateIndexWidget);

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

void GitHubRepoBrowser::onSliderChanged(int i)
{
    if(!isSearching_ && hasMore_ && i >= ui->releaseListView->verticalScrollBar()->maximum() - 1000){
        currentPage_ ++;
        getReleaseList(currentPage_);
    }
}

void GitHubRepoBrowser::updateStatusText()
{
    auto str = tr("Loaded %1 releases from GitHub.").arg(model_->rowCount());
    ui->statusbar->showMessage(str);
}

void GitHubRepoBrowser::updateIndexWidget()
{
    auto beginRow = ui->releaseListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->releaseListView->indexAt(QPoint(0, ui->releaseListView->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, 0);
        if(ui->releaseListView->indexWidget(index)) continue;
        auto item = model_->item(row);
        auto release = item->data().value<GitHubRelease*>();
        if(release){
            auto modItemWidget = new GitHubReleaseItemWidget(this, release->info());
//            modItemWidget->setDownloadPath(downloadPath_);
//            connect(this, &ReplayModBrowser::downloadPathChanged, modItemWidget, &ReplayModItemWidget::setDownloadPath);
            ui->releaseListView->setIndexWidget(index, modItemWidget);
            item->setSizeHint(QSize(0, 200));
        }
    }
}

void GitHubRepoBrowser::onItemSelected()
{
    auto indexes = ui->releaseListView->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    auto index = indexes.first();
    auto item = model_->itemFromIndex(index);
    auto release = item->data().value<GitHubRelease*>();
    if(release){
        infoWidget_->setRelease(release);
        fileListWidget_->setRelease(release);
        if(release != selectedRelease_){
            selectedRelease_ = release;
//            emit selectedModsChanged(mod);
        }
    }
}

void GitHubRepoBrowser::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
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
