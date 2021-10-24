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

    items_ << new QTreeWidgetItem({tr("Download")});
    items_ << new QTreeWidgetItem({tr("Explore")});
    items_ << new QTreeWidgetItem({tr("Local")});

    //setup tree widget
    for (const auto &item : qAsConst(items_)){
        item->setForeground(0, QColor(127, 127, 127));
        item->setFlags(item->flags().setFlag(Qt::ItemIsSelectable, false));
        ui->browserTreeWidget->addTopLevelItem(item);
    }
    ui->browserTreeWidget->expandAll();
    connect(ui->browserTreeWidget, &QTreeWidget::customContextMenuRequested, this, &BrowserSelectorWidget::customContextMenuRequested);
}

BrowserSelectorWidget::~BrowserSelectorWidget()
{
    delete ui;
}

QTreeWidget *BrowserSelectorWidget::browserTreeWidget()
{
    return ui->browserTreeWidget;
}

QTreeWidgetItem *BrowserSelectorWidget::item(BrowserCategory category)
{
    return items_[category];
}

void BrowserSelectorWidget::on_browserTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if(!current) return;
    auto parent = current->parent();
    if(!parent) return;
    if(int i = items_.indexOf(parent); i >= 0)
        emit browserChanged(static_cast<BrowserCategory>(i), parent->indexOfChild(current));
}

QTreeWidgetItem *BrowserSelectorWidget::downloadItem()
{
    return items_[Download];
}

QTreeWidgetItem *BrowserSelectorWidget::exploreItem()
{
    return items_[Explore];
}

QTreeWidgetItem *BrowserSelectorWidget::localItem()
{
    return items_[Local];
}

void BrowserSelectorWidget::addMultiple()
{
    auto paths = getExistingDirectories(this, tr("Select your mod directories..."), Config().getCommonPath());
    LocalModPathManager::addPaths(paths);
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
