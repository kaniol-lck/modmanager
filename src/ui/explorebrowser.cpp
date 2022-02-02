#include "browserdialog.h"
#include "downloadpathselectmenu.h"
#include "explorebrowser.h"

#include <QAction>
#include <QDesktopServices>
#include <QDockWidget>
#include <QMenu>
#include <QStandardItem>
#include <QUrl>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QStatusBar>
#include <QStackedWidget>

#include "exploremanager.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/smoothscrollbar.h"

ExploreBrowser::ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url) :
    Browser(parent),
    statusBar_(new QStatusBar(this)),
    statusBarWidget_(new ExploreStatusBarWidget(this)),
    menu_(new QMenu(this)),
    downloadPathSelectMenu_(new DownloadPathSelectMenu(this)),
    stackedWidget_(new QStackedWidget(this)),
    modListView_(new QListView(this)),
    modIconListView_(new QListView(this)),
    modTreeView_(new QTreeView(this)),
    icon_(icon),
    name_(name)
{
    stackedWidget_->addWidget(modListView_);
    stackedWidget_->addWidget(modIconListView_);
    stackedWidget_->addWidget(modTreeView_);
    modIconListView_->setViewMode(QListView::IconMode);
    modTreeView_->hideColumn(0);

    modListView_->setVerticalScrollBar(new SmoothScrollBar(this));
    modListView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    modListView_->setProperty("class", "ModList");
    modListView_->setContextMenuPolicy(Qt::CustomContextMenu);

    setStatusBar(statusBar_);
    statusBar_->addPermanentWidget(statusBarWidget_);
    connect(statusBarWidget_, &ExploreStatusBarWidget::viewModeChanged, stackedWidget_, &QStackedWidget::setCurrentIndex);

    connect(modListView_, &QWidget::customContextMenuRequested, this, &ExploreBrowser::onCustomContextMenuRequested);
    connect(modListView_, &QAbstractItemView::doubleClicked, this, &ExploreBrowser::onDoubleClicked);
    connect(modListView_->verticalScrollBar(), &QScrollBar::valueChanged, this , &ExploreBrowser::onSliderChanged);
    connect(modListView_->verticalScrollBar(), &QScrollBar::valueChanged, this, &ExploreBrowser::updateIndexWidget);

    refreshAction_ = menu_->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh Mods"), this, &ExploreBrowser::refresh);
    refreshAction_->setShortcut(QKeySequence(Qt::Key_F5));
    visitWebsiteAction_ = menu_->addAction(icon, tr("Visit %1").arg(name), [=]{
        QDesktopServices::openUrl(url);
    });
    openDialogAction_ = menu_->addAction(QIcon::fromTheme("window-new"), tr("Open in New Dialog"), this, [=]{
        another()->show();
    });
    menu_->addMenu(downloadPathSelectMenu_);
}

QAction *ExploreBrowser::visitWebsiteAction() const
{
    return visitWebsiteAction_;
}

QIcon ExploreBrowser::icon() const
{
    return icon_;
}

QString ExploreBrowser::name() const
{
    return name_;
}

void ExploreBrowser::onSliderChanged(int i)
{
    if(i >= modListView_->verticalScrollBar()->maximum() - 1000){
        loadMore();
    }
}

void ExploreBrowser::onItemSelected()
{
    auto indexes = modListView_->selectionModel()->selectedRows();
    if(!indexes.isEmpty()){
        auto index = indexes.first();
        onSelectedItemChanged(index);
    } else
        onSelectedItemChanged(QModelIndex());
}

void ExploreBrowser::updateIndexWidget()
{
    auto beginRow = modListView_->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = modListView_->indexAt(QPoint(0, modListView_->height())).row();
    if(endRow < 0)
        endRow = model()->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model()->rowCount(); row++){
        auto index = model()->index(row, 0);
        if(modListView_->indexWidget(index)) continue;
        if(auto widget = getIndexWidget(index)){
            modListView_->setIndexWidget(index, widget);
//            item->setSizeHint(QSize(0, widget->height()));
        }
    }
}

void ExploreBrowser::onCustomContextMenuRequested(const QPoint &pos)
{
    if(auto menu = getCustomContextMenu())
        menu->exec(modListView_->mapToGlobal(pos));
}

void ExploreBrowser::onDoubleClicked(const QModelIndex &index)
{
    if(auto dialog = getDialog(index))
        dialog->show();
}

void ExploreBrowser::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
}

void ExploreBrowser::initUi(ExploreManager *manager, QAbstractItemModel *model)
{
    manager_ = manager;
    setCentralWidget(stackedWidget_);
    modListView_->setModel(model);
    modIconListView_->setModel(model);
    modTreeView_->setModel(model);
    connect(modListView_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ExploreBrowser::onItemSelected);
    onItemSelected();

    Config config;
    restoreGeometry(config.getBrowserWindowState(this));
    restoreState(config.getBrowserWindowState(this));

    connect(manager, &ExploreManager::searchStarted, this, [=]{
        setCursor(Qt::BusyCursor);
        statusBarWidget_->setText(tr("Searching mods..."));
        statusBarWidget_->setProgressVisible(true);
        refreshAction_->setEnabled(false);
    });
    connect(manager, &ExploreManager::searchFinished, this, [=](bool success){
        setCursor(Qt::ArrowCursor);
        statusBarWidget_->setText(success? "" : tr("Failed loading"));
        statusBarWidget_->setProgressVisible(false);
        refreshAction_->setEnabled(true);
        updateStatusText();
    });
    connect(manager, &ExploreManager::scrollToTop, this, [=]{
        scrollToTop();
    });
}

void ExploreBrowser::initUi(ExploreManager *manager)
{
    initUi(manager, manager->model());
}

void ExploreBrowser::initUi(ExploreManager *manager, QSortFilterProxyModel *proxyModel)
{
    proxyModel_ = proxyModel;
    initUi(manager, qobject_cast<QAbstractItemModel *>(proxyModel));
    connect(manager, &ExploreManager::searchFinished, this, [=](bool){
        proxyModel->invalidate();
        updateStatusText();
    });
}

bool ExploreBrowser::isRowHidden(int row)
{
    return modListView_->isRowHidden(row);
}

void ExploreBrowser::setRowHidden(int row, bool hidden)
{
    modListView_->setRowHidden(row, hidden);
}

void ExploreBrowser::scrollToTop()
{
    modListView_->scrollToTop();
}

void ExploreBrowser::loadMore()
{}

void ExploreBrowser::onSelectedItemChanged(const QModelIndex &index[[maybe_unused]])
{}

QWidget *ExploreBrowser::getIndexWidget(const QModelIndex &index[[maybe_unused]])
{
    return nullptr;
}

QDialog *ExploreBrowser::getDialog(const QModelIndex &index[[maybe_unused]])
{
    return nullptr;
}

QMenu *ExploreBrowser::getCustomContextMenu()
{
    return nullptr;
}

QAbstractItemModel *ExploreBrowser::model() const
{
    return modListView_->model();
}

void ExploreBrowser::updateStatusText()
{
    if(proxyModel_)
        statusBarWidget_->setModCount(proxyModel_->rowCount(), manager_->model()->rowCount());
    else
        statusBarWidget_->setModCount(manager_->model()->rowCount(), manager_->model()->rowCount());
}

QMenu *ExploreBrowser::menu() const
{
    return menu_;
}

DownloadPathSelectMenu *ExploreBrowser::downloadPathSelectMenu() const
{
    return downloadPathSelectMenu_;
}

LocalModPath *ExploreBrowser::downloadPath() const
{
    return downloadPathSelectMenu_->downloadPath();
}
