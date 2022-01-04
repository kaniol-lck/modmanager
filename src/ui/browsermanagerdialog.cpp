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
        ui->browserList->addTopLevelItem(
                    new QTreeWidgetItem(QStringList{path->info().displayName(),
                                                    path->info().path(),
                                                    path->info().gameVersion().toString(),
                                                    ModLoaderType::toString(path->info().loaderType())}));

    ui->browserList->setHeaderItem(new QTreeWidgetItem(QStringList{tr("Name"),
                                                                   tr("Path"),
                                                                   tr("Game Version"),
                                                                   tr("Loader Type")}));
    for(int i = 0; i < 4; i++)
        ui->browserList->resizeColumnToContents(i);
    refreshButton();
}

BrowserManagerDialog::~BrowserManagerDialog()
{
    delete ui;
}

void BrowserManagerDialog::on_upButton_clicked()
{
    auto index = ui->browserList->currentIndex();
    auto row = index.row();
    if(row < 0 || row == 0) return;

    pathList_.swapItemsAt(row, row - 1);
    ui->browserList->insertTopLevelItem(row - 1, ui->browserList->takeTopLevelItem(row));

    ui->browserList->setCurrentIndex(index.siblingAtRow(row - 1));
    refreshButton();
}

void BrowserManagerDialog::on_downButton_clicked()
{
    auto index = ui->browserList->currentIndex();
    auto row = index.row();
    if(row < 0 || row == pathList_.size() - 1) return;

    pathList_.swapItemsAt(row, row + 1);
    ui->browserList->insertTopLevelItem(row, ui->browserList->takeTopLevelItem(row + 1));

    ui->browserList->setCurrentIndex(index.siblingAtRow(row + 1));
    refreshButton();
}

void BrowserManagerDialog::on_addButton_clicked()
{
    auto dialog = new LocalModPathSettingsDialog(this);
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &pathInfo, bool autoLoaderType){
        auto path = new LocalModPath(pathInfo);
        path->loadMods(autoLoaderType);
        pathList_ << path;
        ui->browserList->addTopLevelItem(
                    new QTreeWidgetItem(QStringList{path->info().displayName(),
                                                    path->info().path(),
                                                    path->info().gameVersion().toString(),
                                                    ModLoaderType::toString(path->info().loaderType())}));
        refreshButton();
    });
    dialog->exec();
}

void BrowserManagerDialog::on_deleteButton_clicked()
{
    auto index = ui->browserList->currentIndex();
    auto row = index.row();
    if(row < 0) return;

    pathList_.removeAt(row);
    delete ui->browserList->takeTopLevelItem(row);
    refreshButton();
}

void BrowserManagerDialog::on_BrowserManagerDialog_accepted()
{
    LocalModPathManager::setPathList(pathList_);
}

void BrowserManagerDialog::refreshButton()
{
    on_browserList_currentItemChanged(ui->browserList->currentItem());
}

void BrowserManagerDialog::on_browserList_doubleClicked(const QModelIndex &index)
{
    auto row = index.row();
    if(row < 0) return;

    auto path = pathList_.at(row);
    auto dialog = new LocalModPathSettingsDialog(this, path->info());
    connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo, bool autoLoaderType){
        pathList_[row]->setInfo(newInfo, autoLoaderType);
        ui->browserList->topLevelItem(row)->setText(0, newInfo.displayName());
        ui->browserList->topLevelItem(row)->setText(1, newInfo.path());
        ui->browserList->topLevelItem(row)->setText(2, newInfo.gameVersion().toString());
        ui->browserList->topLevelItem(row)->setText(3, ModLoaderType::toString(newInfo.loaderType()));
    });
    dialog->exec();
}

void BrowserManagerDialog::on_browserList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous[[maybe_unused]])
{
    auto currentRow = ui->browserList->indexOfTopLevelItem(current);
    ui->upButton->setEnabled(currentRow >= 0 && currentRow != 0);
    ui->downButton->setEnabled(currentRow >= 0 && currentRow != pathList_.size() - 1);
    ui->deleteButton->setEnabled(currentRow >= 0);
}
