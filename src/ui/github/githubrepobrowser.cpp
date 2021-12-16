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

GitHubRepoBrowser::GitHubRepoBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/github.svg"), "GitHub", QUrl("https://www.github.com")),
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

    connect(ui->releaseListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &GitHubRepoBrowser::onItemSelected);
    connect(ui->releaseListView->verticalScrollBar(), &QScrollBar::valueChanged, this, &GitHubRepoBrowser::updateIndexWidget);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getRepoList();
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
        getRepoList();
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
    return new GitHubRepoBrowser;
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

void GitHubRepoBrowser::getRepoList()
{
    setCursor(Qt::BusyCursor);
    statusBarWidget_->setText(tr("Searching mods..."));
    statusBarWidget_->setProgressVisible(true);

    auto conn = api_->getReleases(QUrl("https://api.github.com/repos/kaniol-lck/modmanager/releases"), [=](const auto &list){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText("");
        statusBarWidget_->setProgressVisible(false);
//        for(auto row = 0; row < model_->rowCount(); row++){
//            auto item = model_->item(row);
//            auto mod = item->data().value<ReplayMod*>();
//            if(mod && !mod->parent())
//                mod->deleteLater();
//        }
//        model_->clear();
//        QStringList gameVersions;
        for(auto releaseInfo : list){
            auto release = new GitHubRelease(this, releaseInfo);
//            if(!gameVersions.contains(modInfo.gameVersion()))
//                gameVersions << modInfo.gameVersion();
//            auto mod = new ReplayMod(this, modInfo);
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(release));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
        }
//        ui->versionSelect->clear();
//        ui->versionSelect->addItem(tr("Any"));
//        ui->versionSelect->addItems(gameVersions);

        auto item = new QStandardItem(tr("There is no more mod here..."));
        item->setSizeHint(QSize(0, 108));
        auto font = qApp->font();
        font.setPointSize(20);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model_->appendRow(item);

        updateStatusText();
    });

}
