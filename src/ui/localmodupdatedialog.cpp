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

    ui->updateTableView->horizontalHeader()->setSectionResizeMode(SourceColumn, QHeaderView::Fixed);
    ui->updateTableView->setColumnWidth(SourceColumn, 140);

    for(const auto &mod : modPath_->modMap()){
        auto type = mod->defaultUpdateType();
        if(type == LocalMod::None) continue;
        auto names = mod->updateNames(type);

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->modInfo().fabric().name());
        nameItem->setCheckable(true);
        nameItem->setCheckState(Qt::Checked);
        nameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        nameItem->setEditable(false);

        auto beforeItem = new QStandardItem();
        beforeItem->setText(names.first);
        beforeItem->setForeground(Qt::red);
        beforeItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        beforeItem->setEditable(false);

        auto afterItem = new QStandardItem();
        afterItem->setText(names.second);
        afterItem->setForeground(Qt::green);
        afterItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        afterItem->setEditable(false);

        auto sourceItem = new QStandardItem();
        auto pair = UpdateSourceDelegate::sourceItems.at(type);
        sourceItem->setText(pair.first);
        sourceItem->setIcon(QIcon(pair.second));
        sourceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

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

