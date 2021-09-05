#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include <tuple>

LocalModUpdateDialog::LocalModUpdateDialog(QWidget *parent, const QList<LocalMod *> &list) :
    QDialog(parent),
    ui(new Ui::LocalModUpdateDialog)
{
    ui->setupUi(this);
    ui->updateTableView->setModel(&model_);
    ui->updateTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(BeforeColumn, new QStandardItem(tr("Old Version")));
    model_.setHorizontalHeaderItem(AfterColumn, new QStandardItem(tr("New Version")));
    model_.setHorizontalHeaderItem(SourceColumn, new QStandardItem(tr("Update Source")));

    for(const auto &mod : list){
        auto type = mod->updateType();
        if(type == LocalMod::None) continue;

        static auto getDisplayName = [=](const auto &fileInfo){
            return fileInfo.displayName();
        };

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->modInfo().name() + ":");
        nameItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        nameItem->setCheckable(true);
        nameItem->setCheckState(Qt::Checked);

        auto beforeItem = new QStandardItem();
        if(type == LocalMod::Curseforge)
            beforeItem->setText(getDisplayName(mod->currentCurseforgeFileInfo().value()));
        else if(type == LocalMod::Modrinth)
            beforeItem->setText(getDisplayName(mod->currentModrinthFileInfo().value()));
        beforeItem->setForeground(Qt::red);

        auto afterItem = new QStandardItem();
        if(type == LocalMod::Curseforge)
            afterItem->setText(getDisplayName(mod->updateCurseforgeFileInfo().value()));
        else if(type == LocalMod::Modrinth)
            afterItem->setText(getDisplayName(mod->updateModrinthFileInfo().value()));
        afterItem->setForeground(Qt::green);

        auto sourceItem = new QStandardItem();
        sourceItem->setText(type == LocalMod::Curseforge? "Curseforge" : "Modrinth");
        sourceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        model_.appendRow({nameItem, beforeItem, afterItem, sourceItem});
        updateList_ << mod;
    }
}

LocalModUpdateDialog::~LocalModUpdateDialog()
{
    delete ui;
}


void LocalModUpdateDialog::on_LocalModUpdateDialog_accepted()
{
    for(int row = 0; row < model_.rowCount(); row++)
        if(model_.item(row)->checkState() == Qt::Checked)
            updateList_.at(row)->update(updateList_.at(row)->updateType());
}

