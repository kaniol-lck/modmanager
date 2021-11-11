#include "browserselectorwidget.h"
#include "ui_browserselectorwidget.h"

#include <QAction>
#include <QFileDialog>
#include <QListView>

#include "ui/local/localmodpathsettingsdialog.h"
#include "ui/browsermanagerdialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "util/funcutil.h"

BrowserSelectorWidget::BrowserSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserSelectorWidget)
{
    ui->setupUi(this);
    auto action = new QAction(tr("Add multiple paths"), this);
    connect(action, &QAction::triggered, this, &BrowserSelectorWidget::addMultiple, Qt::QueuedConnection);
    ui->addButton->addAction(action);

//    connect(ui->browserTreeView, &QTreeView::entered, this, &BrowserSelectorWidget::onItemSelected);
    connect(ui->browserTreeView, &QTreeView::clicked, this, &BrowserSelectorWidget::onItemSelected);
}

BrowserSelectorWidget::~BrowserSelectorWidget()
{
    delete ui;
}

void BrowserSelectorWidget::setModel(QAbstractItemModel *model)
{
    ui->browserTreeView->setModel(model);
    ui->browserTreeView->expandAll();
}

void BrowserSelectorWidget::setCurrentIndex(QModelIndex index)
{
    ui->browserTreeView->setCurrentIndex(index);
}

void BrowserSelectorWidget::addMultiple()
{
    auto paths = getExistingDirectories(this, tr("Select your mod directories..."), Config().getCommonPath());
    LocalModPathManager::addPaths(paths);
}

void BrowserSelectorWidget::onItemSelected(const QModelIndex &index)
{
    if(auto parent = index.parent(); parent.isValid())
        emit browserChanged(parent.row(), index.row());
}

void BrowserSelectorWidget::on_addButton_clicked()
{
    auto dialog = new LocalModPathSettingsDialog(this);
    dialog->exec();
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
        auto path = new LocalModPath(pathInfo, autoLoaderType);
        LocalModPathManager::addPath(path);
    });
}

void BrowserSelectorWidget::on_manageButton_clicked()
{
    auto dialog = new BrowserManagerDialog(this);
    dialog->show();
}

void BrowserSelectorWidget::on_browserTreeView_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui->browserTreeView->indexAt(pos);
    emit customContextMenuRequested(index, ui->browserTreeView->mapToGlobal(pos));
}
