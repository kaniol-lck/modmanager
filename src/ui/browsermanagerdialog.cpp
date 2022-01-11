#include "browsermanagerdialog.h"
#include "ui_browsermanagerdialog.h"

#include <QListWidgetItem>

#include "local/localmodpathsettingsdialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "util/funcutil.h"
#include "config.hpp"

BrowserManagerDialog::BrowserManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowserManagerDialog)
{
    ui->setupUi(this);
    pathList_ = LocalModPathManager::pathList();

    for(const auto &path : qAsConst(pathList_)){
        auto item = new QTreeWidgetItem(QStringList{path->info().displayName(),
                                                    path->info().gameVersion().toString(),
                                                    ModLoaderType::toString(path->info().loaderType()),
                                                    path->info().path()});
        item->setIcon(2, ModLoaderType::icon(path->info().loaderType()));
        ui->browserList->addTopLevelItem(item);
    }

    ui->browserList->setHeaderItem(new QTreeWidgetItem(QStringList{tr("Name"),
                                                                   tr("Game Version"),
                                                                   tr("Loader Type"),
                                                                   tr("Path")}));
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
        auto item = new QTreeWidgetItem(QStringList{path->info().displayName(),
                                                    path->info().gameVersion().toString(),
                                                    ModLoaderType::toString(path->info().loaderType()),
                                                    path->info().path()});
        item->setIcon(2, ModLoaderType::icon(path->info().loaderType()));
        ui->browserList->addTopLevelItem(item);
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
        ui->browserList->topLevelItem(row)->setText(1, newInfo.gameVersion().toString());
        ui->browserList->topLevelItem(row)->setText(2, ModLoaderType::toString(newInfo.loaderType()));
        ui->browserList->topLevelItem(row)->setIcon(2, ModLoaderType::icon(newInfo.loaderType()));
        ui->browserList->topLevelItem(row)->setText(3, newInfo.path());
    });
    dialog->exec();
}

void BrowserManagerDialog::on_browserList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous[[maybe_unused]])
{
    auto currentRow = ui->browserList->indexOfTopLevelItem(current);
    ui->upButton->setEnabled(currentRow >= 0 && currentRow != 0);
    ui->downButton->setEnabled(currentRow >= 0 && currentRow != pathList_.size() - 1);
    ui->openFolderButton->setEnabled(currentRow >= 0);
    ui->editButton->setEnabled(currentRow >= 0);
    ui->deleteButton->setEnabled(currentRow >= 0);
}

void BrowserManagerDialog::on_editButton_clicked()
{
    on_browserList_doubleClicked(ui->browserList->currentIndex());
}

void BrowserManagerDialog::on_openFolderButton_clicked()
{
    auto row = ui->browserList->currentIndex().row();
    if(row < 0) return;
    if(auto path = pathList_.at(row))
        openFolder(path->info().path());
}

