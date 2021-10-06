#include "browserselectorwidget.h"
#include "ui_browserselectorwidget.h"

#include "ui/local/localmodpathsettingsdialog.h"
#include "ui/browsermanagerdialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"

BrowserSelectorWidget::BrowserSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserSelectorWidget)
{
    ui->setupUi(this);

    items_ << new QTreeWidgetItem({tr("Download")});
    items_ << new QTreeWidgetItem({tr("Explore")});
    items_ << new QTreeWidgetItem({tr("Local")});

    //setup tree widgetl
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

void BrowserSelectorWidget::on_addButton_clicked()
{
    auto dialog = new LocalModPathSettingsDialog(this);
    dialog->show();
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo){
        auto path = new LocalModPath(this, pathInfo);
        LocalModPathManager::addPath(path);
    });
}

void BrowserSelectorWidget::on_manageButton_clicked()
{
    auto dialog = new BrowserManagerDialog(this);
    dialog->show();
}
