#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

#include <QInputDialog>
#include <QDebug>
#include <QToolBar>
#include <QClipboard>

#include "downloadstatusbarwidget.h"
#include "ui/download/qaria2downloaderitemwidget.h"
#include "download/downloadmanager.h"
#include "download/qaria2.h"
#include "download/qaria2downloader.h"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "adddownloaddialog.h"
#include "downloaderinfowidget.h"

DownloadBrowser::DownloadBrowser(QWidget *parent) :
    Browser(parent),
    ui(new Ui::DownloadBrowser),
    manager_(DownloadManager::manager()),
    statusBarWidget_(new DownloadStatusBarWidget(this)),
    infoWidget_(new DownloaderInfoWidget(this, manager_->qaria2()))
{
    ui->setupUi(this);
    ui->statusbar->addPermanentWidget(statusBarWidget_);

    for(auto &&toolBar : findChildren<QToolBar *>())
        ui->menu_View->addAction(toolBar->toggleViewAction());

    Config config;
    restoreGeometry(config.getBrowserWindowState(this));
    restoreState(config.getBrowserWindowState(this));
    ui->downloaderListView->setModel(manager_->model());
    ui->downloaderListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->downloaderListView->setProperty("class", "ModList");

    connect(manager_->qaria2(), &QAria2::started, this, [=]{});
    connect(manager_->qaria2(), &QAria2::finished, this, [=]{
        ui->statusbar->showMessage("Idoling");
    });
    connect(manager_->qaria2(), &QAria2::downloadSpeed, statusBarWidget_, &DownloadStatusBarWidget::setDownloadSpeed);
    onSelectionChanged();
    connect(ui->downloaderListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DownloadBrowser::onSelectionChanged);
}

DownloadBrowser::~DownloadBrowser()
{
    delete ui;
}

QWidget *DownloadBrowser::infoWidget() const
{
    return infoWidget_;
}

QIcon DownloadBrowser::icon() const
{
    return QIcon::fromTheme("download");
}

QString DownloadBrowser::name() const
{
    return tr("Downloader");
}

void DownloadBrowser::onSelectionChanged()
{
    disconnect(conn_);
    QList<AbstractDownloader *> list;
    for(auto &&index : ui->downloaderListView->selectionModel()->selectedRows())
        list << manager_->model()->data(index, Qt::UserRole + 1).value<AbstractDownloader*>();
    if(list.isEmpty()){
        ui->actionPause->setEnabled(false);
        ui->actionForce_Pause->setEnabled(false);
        ui->actionStart->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionForce_Stop->setEnabled(false);
        ui->actionCopy_Download_Link->setEnabled(false);
        ui->actionShow_in_Folder->setEnabled(false);
        infoWidget_->setDownloader(nullptr);
        return;
    }
    ui->actionCopy_Download_Link->setEnabled(true);
    ui->actionShow_in_Folder->setEnabled(true);
    auto downloader = list.first();
    infoWidget_->setDownloader(downloader);
    auto updateButtons = [=]{
        ui->actionPause->setEnabled(downloader->isStarted());
        ui->actionForce_Pause->setEnabled(downloader->isStarted());
        ui->actionStart->setEnabled(downloader->isPaused());
        ui->actionStop->setEnabled(!downloader->isStopped());
        ui->actionForce_Stop->setEnabled(!downloader->isStopped());
    };
    updateButtons();
    conn_ = connect(downloader, &AbstractDownloader::statusChanged, this, updateButtons);
}

void DownloadBrowser::on_actionAdd_triggered()
{
    auto dialog = new AddDownloadDialog(this);
    dialog->show();
}

void DownloadBrowser::updateListViewIndexWidget()
{
    auto &&model = manager_->model();
    auto beginRow = ui->downloaderListView->indexAt(QPoint(0, 0)).row();
//    if(beginRow < 0) return;
    auto endRow = ui->downloaderListView->indexAt(QPoint(0, ui->downloaderListView->height())).row();
    if(endRow < 0)
        endRow = model->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model->rowCount(); row++){
        auto index = model->index(row, 0);
        if(ui->downloaderListView->indexWidget(index)) continue;
//        qDebug() << "new widget at row" << row;
        auto downloader = model->data(index, Qt::UserRole + 1).value<QAria2Downloader *>();
        if(downloader){
            auto modItemWidget = new QAria2DownloaderItemWidget(ui->downloaderListView, downloader);
            ui->downloaderListView->setIndexWidget(index, modItemWidget);
            model->setItemHeight(modItemWidget->height());
        }
    }
}

void DownloadBrowser::paintEvent(QPaintEvent *event)
{
    updateListViewIndexWidget();
    QWidget::paintEvent(event);
}

void DownloadBrowser::on_actionPause_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    if(downloader) downloader->pause();
}

void DownloadBrowser::on_actionStart_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    if(downloader) downloader->start();
}

void DownloadBrowser::on_actionCopy_Download_Link_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    QApplication::clipboard()->setText(downloader->info().url().toString());
}

void DownloadBrowser::on_actionShow_in_Folder_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    openFileInFolder(downloader->info().fileName(), downloader->info().path());
}

void DownloadBrowser::on_downloaderListView_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    for(auto &&action : ui->menu_Download->actions())
        if(action->isEnabled()) menu->addAction(action);
    menu->exec(ui->downloaderListView->viewport()->mapToGlobal(pos));
}

void DownloadBrowser::on_actionForce_Pause_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    if(downloader) downloader->pause(true);

}

void DownloadBrowser::on_actionStop_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    if(downloader) downloader->stop();
}

void DownloadBrowser::on_actionForce_Stop_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->downloaders().at(row);
    if(downloader) downloader->stop(true);

}
