#include "curseforgefilelistwidget.h"
#include "ui_curseforgefilelistwidget.h"

#include <QStandardItemModel>

#include "ui/downloadpathselectmenu.h"
#include "curseforgemodbrowser.h"
#include "curseforgefileitemwidget.h"
#include "curseforge/curseforgemod.h"
#include "local/localmodpath.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

CurseforgeFileListWidget::CurseforgeFileListWidget(CurseforgeModBrowser *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileListWidget),
    versionMenu_(new QMenu(this))
{
    ui->setupUi(this);
    ui->versionSelect->setDefaultAction(versionMenu_->menuAction());
    ui->versionSelect->setPopupMode(QToolButton::InstantPopup);
    ui->fileListView->setModel(&model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListView->setProperty("class", "ModList");
    connect(ui->fileListView->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeFileListWidget::updateIndexWidget);
    connect(ui->fileListView->verticalScrollBar(), &QSlider::valueChanged, this, &CurseforgeFileListWidget::onListSliderChanged);
    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeFileListWidget::updateVersionList);
    ui->downloadPathSelect->hide();
    downloadPathSelectMenu_ = parent->downloadPathSelectMenu();
}

CurseforgeFileListWidget::CurseforgeFileListWidget(QWidget *parent, LocalMod *localMod) :
    QWidget(parent),
    ui(new Ui::CurseforgeFileListWidget),
    versionMenu_(new QMenu(this))
{
    ui->setupUi(this);
    ui->versionSelect->setDefaultAction(versionMenu_->menuAction());
    ui->versionSelect->setPopupMode(QToolButton::InstantPopup);
    ui->fileListView->setModel(&model_);
    ui->fileListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListView->setProperty("class", "ModList");
    connect(ui->fileListView->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &CurseforgeFileListWidget::updateIndexWidget);
    connect(ui->fileListView->verticalScrollBar(), &QSlider::valueChanged, this, &CurseforgeFileListWidget::onListSliderChanged);
    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::curseforgeVersionListUpdated, this, &CurseforgeFileListWidget::updateVersionList);
    ui->downloadPathSelect->hide();
    downloadPathSelectMenu_ = new DownloadPathSelectMenu(this);
    ui->downloadPathSelect->setDefaultAction(downloadPathSelectMenu_->menuAction());
    ui->downloadPathSelect->setPopupMode(QToolButton::InstantPopup);
    setLocalMod(localMod);
}

CurseforgeFileListWidget::~CurseforgeFileListWidget()
{
    delete ui;
}

void CurseforgeFileListWidget::setMod(CurseforgeMod *mod)
{
    mod_ = mod;
    emit modChanged();
    ui->fileListView->setVisible(mod_);
    if(!mod_) return;
    connect(this, &CurseforgeFileListWidget::modChanged, disconnecter(
                connect(mod_, &CurseforgeMod::moreFileListReady, this, &CurseforgeFileListWidget::updateFileList),
                connect(mod_, &QObject::destroyed, this, [=]{ setMod(nullptr); })));

    updateFileList();
    search();
}

void CurseforgeFileListWidget::updateUi()
{
    for(auto &&widget : findChildren<CurseforgeFileItemWidget *>())
        widget->updateUi();
}

void CurseforgeFileListWidget::updateFileList()
{
    model_.clear();
    for(int i = 0; i < mod_->modInfo().allFileList().size(); i++){
        auto item = new QStandardItem;
        model_.appendRow(item);
        auto &&fileInfo = mod_->modInfo().allFileList().at(i);
        item->setData(fileInfo.fileDate(), Qt::UserRole);
        item->setData(i, Qt::UserRole + 1);
        item->setSizeHint(QSize(0, 100));
    }
    model_.setSortRole(Qt::UserRole);
    model_.sort(0, Qt::DescendingOrder);
    ui->fileListView->setCursor(Qt::ArrowCursor);
}

void CurseforgeFileListWidget::paintEvent(QPaintEvent *event)
{
    updateIndexWidget();
    QWidget::paintEvent(event);
}

void CurseforgeFileListWidget::setDownloadPathSelectMenu(DownloadPathSelectMenu *newDownloadPathSelectMenu)
{
    downloadPathSelectMenu_ = newDownloadPathSelectMenu;
}

void CurseforgeFileListWidget::setLocalMod(LocalMod *newLocalMod)
{
    localMod_ = newLocalMod;
    if(localMod_){
        downloadPathSelectMenu_->setDownloadPath(localMod_->path());
        ui->downloadPathSelect->show();
    }
}

DownloadPathSelectMenu *CurseforgeFileListWidget::downloadPathSelectMenu() const
{
    return downloadPathSelectMenu_;
}

void CurseforgeFileListWidget::updateIndexWidget()
{
    auto beginRow = ui->fileListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->fileListView->indexAt(QPoint(0, ui->fileListView->height())).row();
    if(endRow < 0)
        endRow = model_.rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_.rowCount(); row++){
        auto index = model_.index(row, 0);
        if(ui->fileListView->indexWidget(index)) continue;
        auto item = model_.item(row);
        auto &&fileInfo = mod_->modInfo().allFileList().at(item->data(Qt::UserRole + 1).toInt());
        auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo);
        ui->fileListView->setIndexWidget(model_.indexFromItem(item), itemWidget);
        item->setSizeHint(QSize(0, itemWidget->height()));
    }
}

void CurseforgeFileListWidget::onListSliderChanged(int i)
{
    if(i >= ui->fileListView->verticalScrollBar()->maximum() - 1000){
        search();
    }
}

void CurseforgeFileListWidget::updateVersionList()
{
    versionMenu_->clear();
    versionMenu_->setTitle(tr("Select Game Version"));
    versionMenu_->setStyleSheet("QMenu { menu-scrollable: 1; }");
    auto anyVersionAction = versionMenu_->addAction(tr("Any"));
    connect(anyVersionAction, &QAction::triggered, this, [=]{
        currentGameVersion_ = GameVersion::Any;
        versionMenu_->setTitle(tr("Game Version : %1").arg(tr("Any")));
        versionMenu_->setIcon(QIcon());
        search(true);
    });
    versionMenu_->addSeparator();
    QMap<QString, QMenu*> submenus;
    QList<QString> keys; //to keep order
    for(auto &&version : GameVersion::curseforgeVersionList()){
        if(!submenus.contains(version.majorVersion())){
            auto submenu = new QMenu(version.majorVersion());
            submenus[version.majorVersion()] = submenu;
            keys << version.majorVersion();
        }
    }
    for(auto &&version : GameVersion::curseforgeVersionList()){
        if(submenus.contains(version.majorVersion())){
            auto submenu = submenus[version.majorVersion()];
            if(submenu->actions().size() == 1)
                if(GameVersion version = submenu->actions().at(0)->text(); version == version.majorVersion())
                    submenu->addSeparator();
            submenu->addAction(version, this, [=]{
                currentGameVersion_ = version;
                versionMenu_->setTitle(tr("Game Version : %1").arg(version));
                search(true);
            });
        }
    }
    for(const auto &key : qAsConst(keys)){
        auto submenu = submenus[key];
        if(submenu->actions().size() == 1)
            versionMenu_->addActions(submenu->actions());
        else
            versionMenu_->addMenu(submenu);
    }
}

void CurseforgeFileListWidget::search(bool changed)
{
    if(changed || !mod_->modInfo().fileCompleted()){
        ui->fileListView->setCursor(Qt::BusyCursor);
        mod_->acquireMoreFileList(currentGameVersion_, changed);
    }
}
