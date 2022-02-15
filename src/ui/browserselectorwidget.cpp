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
    ui->browserTreeView->setProperty("class", "PageSelector");
    auto action = new QAction(tr("Add multiple paths"), this);
    connect(action, &QAction::triggered, this, &BrowserSelectorWidget::addMultiple, Qt::QueuedConnection);
    ui->addButton->addAction(action);
}

BrowserSelectorWidget::~BrowserSelectorWidget()
{
    delete ui;
}

void BrowserSelectorWidget::setModel(QAbstractItemModel *model)
{
    ui->browserTreeView->setModel(model);
    connect(ui->browserTreeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &BrowserSelectorWidget::onCurrentRowChanged);
    ui->browserTreeView->expandAll();
}

void BrowserSelectorWidget::setCurrentIndex(const QModelIndex &index)
{
    ui->browserTreeView->setCurrentIndex(index);
}

void BrowserSelectorWidget::addMultiple()
{
    auto paths = getExistingDirectories(this, tr("Select your mod directories..."), Config().getCommonPath());
    LocalModPathManager::addPaths(paths);
}

void BrowserSelectorWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous[[maybe_unused]])
{
    if(auto parent = current.parent(); parent.isValid())
        emit browserChanged(parent.row(), current.row());
}

void BrowserSelectorWidget::on_addButton_clicked()
{
    auto dialog = new LocalModPathSettingsDialog(this);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
        auto path = new LocalModPath(pathInfo);
        path->loadMods(autoLoaderType);
        LocalModPathManager::addPath(path);
    });
    dialog->exec();
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
