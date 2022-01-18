#include "downloadbrowser.h"
#include "ui_downloadbrowser.h"

#include <QInputDialog>
#include <QDebug>
#include <QToolBar>

#include "ui/download/qaria2downloaderitemwidget.h"
#include "download/downloadmanager.h"
#include "download/qaria2.h"
#include "download/qaria2downloader.h"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "adddownloaddialog.h"

DownloadBrowser::DownloadBrowser(QWidget *parent) :
    Browser(parent),
    ui(new Ui::DownloadBrowser),
    manager_(DownloadManager::manager())
{
    ui->setupUi(this);
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
//    connect(manager_, &DownloadManager::downloaderAdded, this, &DownloadBrowser::addNewDownloaderItem);
    connect(manager_->qaria2(), &QAria2::downloadSpeed, this, &DownloadBrowser::downloadSpeed);
    onCurrentRowChanged();
    connect(ui->downloaderListView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &DownloadBrowser::onCurrentRowChanged);
}

DownloadBrowser::~DownloadBrowser()
{
    delete ui;
}

QIcon DownloadBrowser::icon() const
{
    return QIcon::fromTheme("download");
}

QString DownloadBrowser::name() const
{
    return tr("Downloader");
}

void DownloadBrowser::downloadSpeed(qint64 download, qint64 upload)
{
    auto text = tr("Download Speed:") + speedConvert(download) +
            " " + tr("Upload Speed:") + speedConvert(upload);
    ui->statusbar->showMessage(text);
}

void DownloadBrowser::onCurrentRowChanged()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0){
        ui->actionPause->setEnabled(false);
        ui->actionStart->setEnabled(false);
        return;
    }
    auto downloader = manager_->qaria2()->downloaders().at(row);
    auto updateButtons = [=]{
        ui->actionPause->setEnabled(downloader->status() == aria2::DOWNLOAD_ACTIVE);
        ui->actionStart->setEnabled(downloader->status() == aria2::DOWNLOAD_PAUSED);
    };
    updateButtons();
    disconnect(conn_);
    conn_ = connect(downloader, &QAria2Downloader::statusChanged, this, updateButtons);
}

void DownloadBrowser::on_actionAdd_Download_triggered()
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
    auto downloader = manager_->qaria2()->downloaders().at(row);
    if(downloader) downloader->pause();
}


void DownloadBrowser::on_actionStart_triggered()
{
    auto row = ui->downloaderListView->currentIndex().row();
    if(row < 0) return;
    auto downloader = manager_->qaria2()->downloaders().at(row);
    if(downloader) downloader->start();
}

