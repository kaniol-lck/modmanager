#include "browsermanagerdialog.h"
#include "ui_browsermanagerdialog.h"

#include <QListWidgetItem>
#include "localmodbrowsersettingsdialog.h"
#include "config.h"

BrowserManagerDialog::BrowserManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrowserManagerDialog)
{
    ui->setupUi(this);

    //load mod dirs in config
    Config config;
    for(const auto &variant : config.getDirList()){
        auto modDirInfo = ModDirInfo::fromVariant(variant);
        modDirList << modDirInfo;

        ui->browserList->addItem(modDirInfo.showText());
    }

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

    modDirList.swapItemsAt(row, row - 1);
    ui->browserList->insertItem(row - 1, ui->browserList->takeItem(row));

    ui->browserList->setCurrentRow(row - 1);
    refreshButton();
}


void BrowserManagerDialog::on_downButton_clicked()
{
    auto row = ui->browserList->currentRow();
    if(row < 0 || row == modDirList.size() - 1) return;

    modDirList.swapItemsAt(row, row + 1);
    ui->browserList->insertItem(row, ui->browserList->takeItem(row + 1));

    ui->browserList->setCurrentRow(row + 1);
    refreshButton();
}


void BrowserManagerDialog::on_addButton_clicked()
{
    auto dialog = new LocalModBrowserSettingsDialog(this);
    dialog->show();
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &modDirInfo){
        modDirList << modDirInfo;
        ui->browserList->addItem(modDirInfo.showText());
        refreshButton();
    });
}


void BrowserManagerDialog::on_deleteButton_clicked()
{
    auto row = ui->browserList->currentRow();
    if(row < 0) return;

    modDirList.removeAt(row);
    delete ui->browserList->takeItem(row);
    refreshButton();
}


void BrowserManagerDialog::on_BrowserManagerDialog_accepted()
{
    Config config;
    QList<QVariant> list;
    for(const auto &dirInfo : qAsConst(modDirList))
        list << dirInfo.toVariant();
    config.setDirList(list);
}

void BrowserManagerDialog::on_browserList_currentRowChanged(int currentRow)
{
    ui->upButton->setEnabled(currentRow >= 0 && currentRow != 0);
    ui->downButton->setEnabled(currentRow >= 0 && currentRow != modDirList.size() - 1);
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

    auto modDirInfo = modDirList.at(row);
    auto dialog = new LocalModBrowserSettingsDialog(this, modDirInfo);
    connect(dialog, &LocalModBrowserSettingsDialog::settingsUpdated, this, [=](const ModDirInfo &newInfo){
        modDirList[row] = newInfo;
        ui->browserList->item(row)->setText(newInfo.showText());
    });
    dialog->exec();
}

