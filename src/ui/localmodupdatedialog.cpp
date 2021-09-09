#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "util/updatesourcedelegate.h"

static auto getDisplayName = [](const auto &fileInfo){
    return fileInfo.displayName();
};

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

    for(const auto &mod : modPath_->modList()){
        auto type = mod->updateType();
        if(type == LocalMod::None) continue;

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->modInfo().name());
        nameItem->setCheckable(true);
        nameItem->setCheckState(Qt::Checked);
        nameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        nameItem->setEditable(false);

        auto beforeItem = new QStandardItem();
        if(type == LocalMod::Curseforge)
            beforeItem->setText(getDisplayName(mod->currentCurseforgeFileInfo().value()));
        else if(type == LocalMod::Modrinth)
            beforeItem->setText(getDisplayName(mod->currentModrinthFileInfo().value()));
        beforeItem->setForeground(Qt::red);
        beforeItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        beforeItem->setEditable(false);

        auto afterItem = new QStandardItem();
        if(type == LocalMod::Curseforge)
            afterItem->setText(getDisplayName(mod->updateCurseforgeFileInfo().value()));
        else if(type == LocalMod::Modrinth)
            afterItem->setText(getDisplayName(mod->updateModrinthFileInfo().value()));
        afterItem->setForeground(Qt::green);
        afterItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        afterItem->setEditable(false);

        auto sourceItem = new QStandardItem();
        auto pair = UpdateSourceDelegate::sourceItems.at(type);
        sourceItem->setText(pair.first);
        sourceItem->setIcon(QIcon(pair.second));
        sourceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        model_.appendRow({nameItem, beforeItem, afterItem, sourceItem});
        modUpdateList_ << QPair(mod, mod->updateType());

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

    auto beforeItem = model_.item(row, BeforeColumn);
    auto afterItem = model_.item(row, AfterColumn);
    if(type == LocalMod::Curseforge){
        beforeItem->setText(getDisplayName(mod->currentCurseforgeFileInfo().value()));
        afterItem->setText(getDisplayName(mod->updateCurseforgeFileInfo().value()));
    }
    else if(type == LocalMod::Modrinth){
        beforeItem->setText(getDisplayName(mod->currentModrinthFileInfo().value()));
        afterItem->setText(getDisplayName(mod->updateModrinthFileInfo().value()));
    }
}


void LocalModUpdateDialog::on_LocalModUpdateDialog_accepted()
{
    QList<QPair<LocalMod *, LocalMod::ModWebsiteType>> modUpdateList;
    for(int row = 0; row < model_.rowCount(); row++)
        if(model_.item(row)->checkState() == Qt::Checked)
            modUpdateList << modUpdateList_.at(row);
    modPath_->updateMods(modUpdateList);
}

