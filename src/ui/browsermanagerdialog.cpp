#include "browsermanagerdialog.h"
#include "ui_browsermanagerdialog.h"

#include <QListWidgetItem>

#include "localmodbrowsersettingsdialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "config.h"

BrowserManagerDialog::BrowserManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowserManagerDialog)
{
    ui->setupUi(this);

    ui->upButton->setIcon(QIcon::fromTheme("go-up"));
    ui->downButton->setIcon(QIcon::fromTheme("go-down"));
    ui->addButton->setIcon(QIcon::fromTheme("list-add"));
    ui->deleteButton->setIcon(QIcon::fromTheme("list-remove"));

    pathList_ = LocalModPathManager::pathList();

    for(const auto &path : qAsConst(pathList_))
        ui->browserList->addItem(path->info().showText());

    refreshButton();
}

BrowserManagerDialog::~BrowserManagerDialog()
{
    delete ui;
}

void BrowserManagerDialog::on_upButton_clicked()
{
    auto row = ui->browserList->currentRow();
    if(row < 0 || row == 0) return;

    pathList_.swapItemsAt(row, row - 1);
    ui->browserList->insertItem(row - 1, ui->browserList->takeItem(row));

    ui->browserList->setCurrentRow(row - 1);
    refreshButton();
}


void BrowserManagerDialog::on_downButton_clicked()
{
    auto row = ui->browserList->currentRow();
    if(row < 0 || row == pathList_.size() - 1) return;

    pathList_.swapItemsAt(row, row + 1);
    ui->browserList->insertItem(row, ui->browserList->takeItem(row + 1));

    ui->browserList->setCurrentRow(row + 1);
    refreshButton();
}


void BrowserManagerDialog::on_addButton_clicked()
{
    auto dialog = new LocalModBrowserSettingsDialog(this);
    dialog->show();
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo){
        auto path = new LocalModPath(this, pathInfo);
        pathList_ << path;
        ui->browserList->addItem(pathInfo.showText());
        refreshButton();
    });
}


void BrowserManagerDialog::on_deleteButton_clicked()
{
    auto row = ui->browserList->currentRow();
    if(row < 0) return;

    pathList_.removeAt(row);
    delete ui->browserList->takeItem(row);
    refreshButton();
}


void BrowserManagerDialog::on_BrowserManagerDialog_accepted()
{
    LocalModPathManager::setPathList(pathList_);
}

void BrowserManagerDialog::on_browserList_currentRowChanged(int currentRow)
{
    ui->upButton->setEnabled(currentRow >= 0 && currentRow != 0);
    ui->downButton->setEnabled(currentRow >= 0 && currentRow != pathList_.size() - 1);
    ui->deleteButton->setEnabled(currentRow >= 0);
}

void BrowserManagerDialog::refreshButton()
{
    on_browserList_currentRowChanged(ui->browserList->currentRow());
}

void BrowserManagerDialog::on_browserList_doubleClicked(const QModelIndex &index)
{
    auto row = index.row();
    if(row < 0) return;

    auto path = pathList_.at(row);
    auto dialog = new LocalModBrowserSettingsDialog(this, path->info());
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo){
        pathList_[row]->setInfo(newInfo);
        ui->browserList->item(row)->setText(newInfo.showText());
    });
    dialog->exec();
}

