#include "localmodbrowser.h"
#include "ui_localmodbrowser.h"

#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>

#include "localmodpathsettingsdialog.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "localmoditemwidget.h"
#include "localmoddialog.h"
#include "localmodupdatedialog.h"
#include "localmodcheckdialog.h"
#include "batchrenamedialog.h"
#include "config.h"
#include "util/funcutil.h"
#include "util/localmodsortitem.h"
#include "util/smoothscrollbar.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    QWidget(parent),
    ui(new Ui::LocalModBrowser),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->checkUpdatesProgress->setVisible(false);
    ui->updateAllButton->setVisible(false);
    ui->updateAllButton->setEnabled(false);
    ui->modListWidget->setVerticalScrollBar(new SmoothScrollBar(this));

    auto findNewMenu = new QMenu(this);
    connect(findNewMenu->addAction(QIcon(":/image/curseforge.svg"), "Curseforge"), &QAction::triggered, this, [=]{
        emit findNewOnCurseforge(modPath_->info());
    });
    connect(findNewMenu->addAction(QIcon(":/image/modrinth.svg"), "Modrinth"), &QAction::triggered, this, [=]{
        emit findNewOnModrinth(modPath_->info());
    });
    connect(findNewMenu->addAction(QIcon(":/image/optifine.png"), "OptiFine"), &QAction::triggered, this, [=]{
        emit findNewOnOptifine(modPath_->info());
    });
    connect(findNewMenu->addAction(QIcon(":/image/replay.png"), "ReplayMod"), &QAction::triggered, this, [=]{
        emit findNewOnReplay(modPath_->info());
    });
    ui->findnewButton->setMenu(findNewMenu);

    auto menu = new QMenu(this);
//    connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Edit")), &QAction::triggered, this, [=]{
//        auto pathInfo = modPath_->info();
//        auto dialog = new LocalModPathSettingsDialog(this, pathInfo);
//        connect(dialog, &LocalModPathSettingsDialog::settingsUpdated, this, [=](const LocalModPathInfo &newInfo){
//            modPath_->setInfo(newInfo);
//            localItem_->child(index)->setText(0, newInfo.displayName());
//        });
//        dialog->exec();
//    });
    connect(menu->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh")), &QAction::triggered, this, [=]{
        modPath_->loadMods();
    });
//    connect(menu->addAction(QIcon::fromTheme("delete"), tr("Delete")), &QAction::triggered, this, [=]{
//        if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete this mod path?"))) return;
//        LocalModPathManager::removePath(modPath_);
//    });
//    menu->addSeparator();
    connect(menu->addAction(QIcon::fromTheme("entry-edit"), tr("Batch rename")), &QAction::triggered, this, [=]{
        auto dialog = new BatchRenameDialog(this, modPath_);
        dialog->exec();
    });
    connect(menu->addAction(QIcon::fromTheme("delete"), tr("Delete old")), &QAction::triggered, this, [=]{
        if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete all old file?"))) return;
        modPath_->deleteAllOld();
    });
    ui->menuButton->setMenu(menu);

    updateModList();

    connect(modPath_, &LocalModPath::loadStarted, this, &LocalModBrowser::loadStarted);
    connect(modPath_, &LocalModPath::loadProgress, this, &LocalModBrowser::loadProgress);
    connect(modPath_, &LocalModPath::loadFinished, this, &LocalModBrowser::loadFinished);
    connect(modPath_, &LocalModPath::modListUpdated, this, &LocalModBrowser::updateModList);
    connect(modPath_, &LocalModPath::websiteCheckedCountUpdated, this, &LocalModBrowser::websiteCheckedCountUpdated);
    connect(modPath_, &LocalModPath::checkWebsitesStarted, this, &LocalModBrowser::startCheckWebsites);
    connect(modPath_, &LocalModPath::websitesReady, this, &LocalModBrowser::websitesReady);
    connect(modPath_, &LocalModPath::checkUpdatesStarted, this, &LocalModBrowser::startCheckUpdates);
    connect(modPath_, &LocalModPath::updateCheckedCountUpdated, this, &LocalModBrowser::updateCheckedCountUpdated);
    connect(modPath_, &LocalModPath::updatesReady, this, &LocalModBrowser::updatesReady);
    connect(modPath_, &LocalModPath::updatesStarted, this, &LocalModBrowser::startUpdates);
    connect(modPath_, &LocalModPath::updatesProgress, this, &LocalModBrowser::updatesProgress);
    connect(modPath_, &LocalModPath::updatesDoneCountUpdated, this, &LocalModBrowser::updatesDoneCountUpdated);
    connect(modPath_, &LocalModPath::updatesDone, this, &LocalModBrowser::updatesDone);
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

void LocalModBrowser::updateModList()
{
    ui->modListWidget->clear();
    for (const auto &mod: modPath_->modMap()) {
        auto modItemWidget = new LocalModItemWidget(ui->modListWidget, mod);

        auto *item = new LocalModSortItem(mod);
        item->setSizeHint(QSize(0, 108));

        ui->modListWidget->addItem(item);
        ui->modListWidget->setItemWidget(item, modItemWidget);
    }
    ui->modListWidget->sortItems();
    ui->statusText->setText(tr("%1 mods in total.").arg(modPath_->modMap().size()));
}

void LocalModBrowser::loadStarted()
{
    ui->checkUpdatesProgress->setVisible(true);
    loadProgress(0, 0);
    ui->statusText->setText(tr("Loading mod files..."));
}

void LocalModBrowser::loadProgress(int loadedCount, int totalCount)
{
    ui->checkUpdatesProgress->setVisible(true);
    ui->statusText->setText(tr("Loading mod files.. (Loaded %1/%2 mod files)").arg(loadedCount).arg(totalCount));
    ui->checkUpdatesProgress->setMaximum(totalCount);
    ui->checkUpdatesProgress->setValue(loadedCount);
}

void LocalModBrowser::loadFinished()
{
    ui->checkUpdatesProgress->setVisible(false);
    ui->statusText->setText(tr("All mod files are loaded..."));
}

void LocalModBrowser::startCheckWebsites()
{
    ui->checkUpdatesButton->setEnabled(false);
    websiteCheckedCountUpdated(0);
    ui->checkUpdatesProgress->setVisible(true);
    ui->checkUpdatesButton->setText(tr("Searching on mod websites..."));
    ui->checkUpdatesProgress->setMaximum(modPath_->modMap().size());
}

void LocalModBrowser::websiteCheckedCountUpdated(int checkedCount)
{
    ui->statusText->setText(tr("Searching on mod websites... (Searched %1/%2 mods)").arg(checkedCount).arg(modPath_->modMap().size()));
    ui->checkUpdatesProgress->setValue(checkedCount);
}

void LocalModBrowser::websitesReady()
{
    ui->checkUpdatesButton->setEnabled(true);
    ui->checkUpdatesProgress->setVisible(false);
    ui->statusText->setText(tr("%1 mods in total.").arg(modPath_->modMap().size()));
    ui->checkUpdatesButton->setText(tr("Check updates"));
}

void LocalModBrowser::startCheckUpdates()
{
    ui->checkUpdatesButton->setEnabled(false);
    ui->updateAllButton->setVisible(false);
    updateCheckedCountUpdated(0, 0);
    ui->checkUpdatesProgress->setVisible(true);
    ui->checkUpdatesButton->setText(tr("Checking updates..."));
    ui->checkUpdatesProgress->setMaximum(modPath_->modMap().size());
}

void LocalModBrowser::updateCheckedCountUpdated(int updateCount, int checkedCount)
{
    ui->statusText->setText(tr("%1 mods need update... (Checked %2/%3 mods)").arg(updateCount).arg(checkedCount).arg(modPath_->modMap().size()));
    ui->checkUpdatesProgress->setValue(checkedCount);
}

void LocalModBrowser::updatesReady()
{
    if(isUpdating_) return;
    ui->checkUpdatesButton->setEnabled(true);
    ui->checkUpdatesButton->setText(tr("Check updates"));
    ui->checkUpdatesProgress->setVisible(false);
    if(auto count = modPath_->updatableCount(); count){
        ui->updateAllButton->setVisible(true);
        ui->updateAllButton->setEnabled(true);
        ui->updateAllButton->setText(tr("Update All"));
        ui->statusText->setText(tr("%1 mods need update.").arg(count));
    } else {
        ui->updateAllButton->setVisible(false);
        ui->statusText->setText(tr("%1 mods in total.").arg(modPath_->modMap().size()) + " " + tr("Good! All mods are up-to-date."));
    }
}

void LocalModBrowser::startUpdates()
{
    isUpdating_ = true;
    ui->updateAllButton->setEnabled(false);
    updatesProgress(0, 0);
    ui->checkUpdatesProgress->setVisible(true);
    ui->updateAllButton->setText(tr("Updating..."));
}

void LocalModBrowser::updatesProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->checkUpdatesProgress->setValue(bytesReceived);
    ui->checkUpdatesProgress->setMaximum(bytesTotal);
}

void LocalModBrowser::updatesDoneCountUpdated(int doneCount, int totalCount)
{
    ui->statusText->setText(tr("Updating... (Updated %1/%2 mods)").arg(doneCount).arg(totalCount));
}

void LocalModBrowser::updatesDone(int successCount, int failCount)
{
    isUpdating_ = false;
    auto str = tr("%1 mods in %2 has been updated. Enjoy it!").arg(successCount).arg(modPath_->info().displayName());
    if(failCount)
        str.append("\n").append(tr("Sadly, %1 mods failed to update.").arg(failCount));
    if(Config().getPostUpdate() == Config::Keep)
        str.append("\n").append(tr("You can revert update if find any incompatibility."));
    QMessageBox::information(this, tr("Update Finished"), str);

    updatesReady();
}

void LocalModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = dynamic_cast<const LocalModSortItem*>(ui->modListWidget->item(index.row()))->mod();
    auto dialog = new LocalModDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_searchText_textEdited(const QString &arg1)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto mod = dynamic_cast<const LocalModSortItem*>(ui->modListWidget->item(i))->mod();
        auto str = arg1.toLower();
        if(mod->commonInfo()->name().toLower().contains(str) ||
                mod->commonInfo()->description().toLower().contains(str))
            ui->modListWidget->item(i)->setHidden(false);
        else
            ui->modListWidget->item(i)->setHidden(true);
    }
}

void LocalModBrowser::on_comboBox_currentIndexChanged(int index)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = dynamic_cast<LocalModSortItem*>(ui->modListWidget->item(i));
        item->setSortRule(static_cast<LocalModSortItem::SortRule>(index));
    }
    ui->modListWidget->sortItems();
}

void LocalModBrowser::on_checkUpdatesButton_clicked()
{
    modPath_->checkModUpdates();
}

void LocalModBrowser::on_openFolderButton_clicked()
{
    openFileInFolder(modPath_->info().path());
}

void LocalModBrowser::on_checkButton_clicked()
{
    auto dialog = new LocalModCheckDialog(this, modPath_);
    dialog->exec();
}

void LocalModBrowser::on_updateAllButton_clicked()
{
    auto dialog = new LocalModUpdateDialog(this, modPath_);
    dialog->exec();
}
