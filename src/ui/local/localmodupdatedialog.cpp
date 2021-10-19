#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "util/updatesourcedelegate.h"

LocalModUpdateDialog::LocalModUpdateDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::LocalModUpdateDialog),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->updateTableView->setModel(&model_);
    ui->updateTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(BeforeColumn, new QStandardItem(tr("Old Version")));
    model_.setHorizontalHeaderItem(AfterColumn, new QStandardItem(tr("New Version")));
    model_.setHorizontalHeaderItem(SourceColumn, new QStandardItem(tr("Update Source")));

    ui->updateTableView->horizontalHeader()->setSectionResizeMode(NameColumn, QHeaderView::Fixed);
    ui->updateTableView->setColumnWidth(NameColumn, 250);
    ui->updateTableView->horizontalHeader()->setSectionResizeMode(SourceColumn, QHeaderView::Fixed);
    ui->updateTableView->setColumnWidth(SourceColumn, 140);

    for(const auto &mod : modPath_->modMap()){
        auto enabled = !mod->isDisabled();

        auto type = mod->defaultUpdateType();
        if(type == LocalMod::None) continue;
        auto names = mod->updateNames(type);
        auto infos = mod->updateInfos(type);

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->commonInfo()->name());
        nameItem->setCheckable(enabled);
        nameItem->setCheckState(enabled? Qt::Checked : Qt::Unchecked);
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameItem->setEditable(false);
        nameItem->setEnabled(enabled);

        auto beforeItem = new QStandardItem();
        beforeItem->setText(names.first);
        if(enabled) beforeItem->setForeground(Qt::darkRed);
        beforeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        beforeItem->setEditable(false);
        beforeItem->setEnabled(enabled);
        beforeItem->setToolTip(infos.first);

        auto afterItem = new QStandardItem();
        afterItem->setText(names.second);
        if(enabled) afterItem->setForeground(Qt::darkGreen);
        afterItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        afterItem->setEditable(false);
        afterItem->setEnabled(enabled);
        afterItem->setToolTip(infos.second);

        auto sourceItem = new QStandardItem();
        auto pair = UpdateSourceDelegate::sourceItems.at(type);
        sourceItem->setText(pair.first);
        sourceItem->setIcon(QIcon(pair.second));
        sourceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        sourceItem->setEnabled(enabled);
        if(mod->updateTypes().size() == 1)
            sourceItem->setEnabled(false);

        model_.appendRow({nameItem, beforeItem, afterItem, sourceItem});
        modUpdateList_ << QPair(mod, mod->defaultUpdateType());

        auto updateSourceDelegate = new UpdateSourceDelegate(mod->updateTypes());
        ui->updateTableView->setItemDelegateForRow(model_.rowCount() - 1, updateSourceDelegate);
        connect(updateSourceDelegate, &UpdateSourceDelegate::updateSourceChanged, this, &LocalModUpdateDialog::onUpdateSourceChanged);
    }
}

LocalModUpdateDialog::~LocalModUpdateDialog()
{
    delete ui;
}

void LocalModUpdateDialog::onUpdateSourceChanged(int row, LocalMod::ModWebsiteType type)
{
    modUpdateList_[row].second = type;
    auto mod = modUpdateList_[row].first;
    auto names = mod->updateNames(type);

    auto beforeItem = model_.item(row, BeforeColumn);
    auto afterItem = model_.item(row, AfterColumn);
    beforeItem->setText(names.first);
    afterItem->setText(names.second);
}

void LocalModUpdateDialog::on_LocalModUpdateDialog_accepted()
{
    QList<QPair<LocalMod *, LocalMod::ModWebsiteType>> modUpdateList;
    for(int row = 0; row < model_.rowCount(); row++)
        if(model_.item(row)->checkState() == Qt::Checked)
            modUpdateList << modUpdateList_.at(row);
    modPath_->updateMods(modUpdateList);
}
