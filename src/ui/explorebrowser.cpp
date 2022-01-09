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

#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/smoothscrollbar.h"

ExploreBrowser::ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url) :
    Browser(parent),
    statusBarWidget_(new ExploreStatusBarWidget(this)),
    model_(new QStandardItemModel(this)),
    modMenu_(new QMenu(this)),
    pathMenu_(new QMenu(this)),
    downloadPathSelectMenu_(new DownloadPathSelectMenu(this)),
    modListView_(new QListView(this)),
    icon_(icon),
    name_(name),
    visitWebsiteAction_(new QAction(icon, tr("Visit %1").arg(name), this))
{
    modListView_->setModel(model_);
    modListView_->setVerticalScrollBar(new SmoothScrollBar(this));
    modListView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    modListView_->setProperty("class", "ModList");
    modListView_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(modListView_, &QWidget::customContextMenuRequested, this, &ExploreBrowser::onCustomContextMenuRequested);
    connect(modListView_, &QAbstractItemView::doubleClicked, this, &ExploreBrowser::onDoubleClicked);
    connect(modListView_->verticalScrollBar(), &QScrollBar::valueChanged, this , &ExploreBrowser::onSliderChanged);
    connect(modListView_->verticalScrollBar(), &QScrollBar::valueChanged, this, &ExploreBrowser::updateIndexWidget);
    connect(modListView_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ExploreBrowser::onItemSelected);

    connect(visitWebsiteAction_, &QAction::triggered, this, [=]{
        QDesktopServices::openUrl(url);
    });
    pathMenu_->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh Mods"), this, &ExploreBrowser::refresh)
            ->setShortcut(QKeySequence(Qt::Key_F5));
    pathMenu_->addAction(visitWebsiteAction_);
    pathMenu_->addAction(QIcon::fromTheme("window-new"), tr("Open in New Dialog"), this, &ExploreBrowser::openDialog);
    pathMenu_->addMenu(downloadPathSelectMenu_);
}

QList<QAction *> ExploreBrowser::modActions() const
{
    return modMenu_->actions();
}

QList<QAction *> ExploreBrowser::pathActions() const
{
    return pathMenu_->actions();
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

void ExploreBrowser::openDialog()
{
//    auto dialog = new BrowserDialog(nullptr, another());
//    dialog->setWindowTitle(name_);
//    dialog->setWindowIcon(icon_);
//    dialog->show();
    another()->show();
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
        auto item = model_->itemFromIndex(index);
        onSelectedItemChanged(item);
    } else
        onSelectedItemChanged(nullptr);
}

void ExploreBrowser::updateIndexWidget()
{
    auto beginRow = modListView_->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = modListView_->indexAt(QPoint(0, modListView_->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, 0);
        if(modListView_->indexWidget(index)) continue;
        auto item = model_->item(row);
        if(auto widget = getIndexWidget(item)){
            modListView_->setIndexWidget(index, widget);
            item->setSizeHint(QSize(0, widget->height()));
        }
    }
}

void ExploreBrowser::onCustomContextMenuRequested(const QPoint &pos)
{
    if(auto menu = getMenu())
        menu->exec(modListView_->mapToGlobal(pos));
}

void ExploreBrowser::onDoubleClicked(const QModelIndex &index)
{
    auto item = model_->itemFromIndex(index);
    if(auto dialog = getDialog(item))
        dialog->show();
}

void ExploreBrowser::updateLocalPathList()
{
//    //remember selected path
//    LocalModPath *selectedPath = nullptr;
//    auto index = downloadPathSelect_->currentIndex();
//    if(index >= 0 && index < LocalModPathManager::pathList().size())
//        selectedPath = LocalModPathManager::pathList().at(downloadPathSelect_->currentIndex());

//    downloadPathSelect_->clear();
//    downloadPathSelect_->addItem(tr("Custom"));
//    for(const auto &path : LocalModPathManager::pathList())
//        downloadPathSelect_->addItem(path->info().displayName());

//    //reset selected path
//    if(selectedPath != nullptr){
//        auto index = LocalModPathManager::pathList().indexOf(selectedPath);
//        if(index >= 0)
//            downloadPathSelect_->setCurrentIndex(index);
//    }
}

void ExploreBrowser::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
}

void ExploreBrowser::initUi()
{
//    downloadPathSelect_ = downloadPathSelect;
    setCentralWidget(modListView_);
    onItemSelected();
//    if(downloadPathSelect_){
//        updateLocalPathList();
//        connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ExploreBrowser::updateLocalPathList);
//        connect(downloadPathSelect_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExploreBrowser::setDownloadPathIndex);
//    }
    Config config;
    restoreGeometry(config.getBrowserWindowState(this));
    restoreState(config.getBrowserWindowState(this));
}

void ExploreBrowser::setDownloadPathIndex(int i)
{
//    if(i < 0 || i - 1 >= LocalModPathManager::pathList().count()) return;
//    if(i == 0)
//        downloadPath_ = nullptr;
//    else
//        downloadPath_ =  LocalModPathManager::pathList().at(i - 1);
//    emit downloadPathChanged(downloadPath_);
}

bool ExploreBrowser::isRowHidden(int row)
{
    return modListView_->isRowHidden(row);
}

void ExploreBrowser::setRowHidden(int row, bool hidden)
{
    modListView_->setRowHidden(row, hidden);
}

void ExploreBrowser::loadMore()
{}

void ExploreBrowser::onSelectedItemChanged(QStandardItem *item[[maybe_unused]])
{}

QWidget *ExploreBrowser::getIndexWidget(QStandardItem *item[[maybe_unused]])
{
    return nullptr;
}

QDialog *ExploreBrowser::getDialog(QStandardItem *item[[maybe_unused]])
{
    return nullptr;
}

QMenu *ExploreBrowser::getMenu()
{
    return nullptr;
}

LocalModPath *ExploreBrowser::downloadPath() const
{
    return downloadPathSelectMenu_->downloadPath();
}
