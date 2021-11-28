#include "browsermanagerdialog.h"
#include "ui_browsermanagerdialog.h"

#include <QListWidgetItem>

#include "local/localmodpathsettingsdialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "config.hpp"

BrowserManagerDialog::BrowserManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowserManagerDialog)
{
    ui->setupUi(this);

    pathList_ = LocalModPathManager::pathList();

    for(const auto &path : qAsConst(pathList_))
        ui->browserList->addItem(path->info().displayName());

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
    auto dialog = new LocalModPathSettingsDialog(this);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
        auto path = new LocalModPath(pathInfo);
        path->loadMods(autoLoaderType);
        pathList_ << path;
        ui->browserList->addItem(pathInfo.displayName());
        refreshButton();
    });
    dialog->exec();
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
    auto dialog = new LocalModPathSettingsDialog(this, path->info());
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo, bool autoLoaderType){
        pathList_[row]->setInfo(newInfo, autoLoaderType);
        ui->browserList->item(row)->setText(newInfo.displayName());
    });
    dialog->exec();
}

