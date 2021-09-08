#include "localmodbrowser.h"
#include "ui_localmodbrowser.h"

#include <QDebug>

#include "local/localmodpath.h"
#include "localmoditemwidget.h"
#include "localmodinfodialog.h"
#include "localmodupdatedialog.h"
#include "config.h"
#include "util/localmodsortitem.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    QWidget(parent),
    ui(new Ui::LocalModBrowser),
    modPath_(modPath)
{
    ui->setupUi(this);

    updateModList();
    connect(modPath_, &LocalModPath::modListUpdated, this, &LocalModBrowser::updateModList);
    connect(modPath_, &LocalModPath::websiteCheckedCountUpdated, this, &LocalModBrowser::websiteCheckedCountUpdated);
    connect(modPath_, &LocalModPath::checkWebsitesStarted, this, &LocalModBrowser::startCheckWebsites);
    connect(modPath_, &LocalModPath::websitesReady, this, &LocalModBrowser::websitesReady);
    connect(modPath_, &LocalModPath::checkUpdatesStarted, this, &LocalModBrowser::startCheckUpdates);
    connect(modPath_, &LocalModPath::updateCheckedCountUpdated, this, &LocalModBrowser::updateCheckedCountUpdated);
    connect(modPath_, &LocalModPath::updatesReady, this, &LocalModBrowser::updatesReady);
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

void LocalModBrowser::updateModList()
{
    ui->modListWidget->clear();
    for (const auto &mod: modPath_->modList()) {
        auto modItemWidget = new LocalModItemWidget(ui->modListWidget, mod);

        auto *item = new LocalModSortItem(mod);
        item->setSizeHint(QSize(500, 100));

        ui->modListWidget->addItem(item);
        ui->modListWidget->setItemWidget(item, modItemWidget);
    }
    ui->modListWidget->sortItems();
}

void LocalModBrowser::startCheckWebsites()
{
    ui->checkUpdatesButton->setEnabled(false);
    ui->checkUpdatesButton->setText(tr("Search on mod websites..."));
}

void LocalModBrowser::websiteCheckedCountUpdated(int checkedCount)
{
    ui->checkUpdatesButton->setText(tr("Search on mod websites... ( %1/%2 )").arg(checkedCount).arg(modPath_->modList().size()));
}

void LocalModBrowser::websitesReady()
{
    status_ = CheckUpdates;
    ui->checkUpdatesButton->setEnabled(true);
    ui->checkUpdatesButton->setText(tr("Check updates"));
}

void LocalModBrowser::startCheckUpdates()
{
    ui->checkUpdatesButton->setEnabled(false);
    ui->checkUpdatesButton->setText(tr("Checking updates..."));
}

void LocalModBrowser::updateCheckedCountUpdated(int updateCount, int checkedCount)
{
    ui->checkUpdatesButton->setText(tr("%1 mods need update. (Checking %2/%3 ...)").arg(updateCount).arg(checkedCount).arg(modPath_->modList().size()));
}

void LocalModBrowser::updatesReady(int updateCount)
{
    status_ = ReadyUpdate;
    if(updateCount){
        ui->checkUpdatesButton->setEnabled(true);
        ui->checkUpdatesButton->setText(tr("Update %1 mods").arg(updateCount));
    } else {
        ui->checkUpdatesButton->setText(tr("Good! All mods are up-to-date."));
    }
}

void LocalModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = dynamic_cast<const LocalModSortItem*>(ui->modListWidget->item(index.row()))->mod();
    auto dialog = new LocalModInfoDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_searchText_textEdited(const QString &arg1)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto mod = dynamic_cast<const LocalModSortItem*>(ui->modListWidget->item(i))->mod();
        auto str = arg1.toLower();
        if(mod->modInfo().name().toLower().contains(str) ||
                mod->modInfo().description().toLower().contains(str))
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
    switch (status_) {
    case SearchOnWebsites:
        modPath_->searchOnWebsites();
        break;
    case CheckUpdates:
        modPath_->checkModUpdates();
        break;
    case ReadyUpdate:
        auto dialog = new LocalModUpdateDialog(this, modPath_);
        dialog->show();
        break;
    }
}

