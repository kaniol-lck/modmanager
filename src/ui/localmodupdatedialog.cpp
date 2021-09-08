#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include <tuple>

#include "local/localmodpath.h"
#include "local/localmod.h"

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
    ui->updateTableView->setColumnWidth(SourceColumn, 120);

    for(const auto &mod : modPath_->modList()){
        auto type = mod->updateType();
        if(type == LocalMod::None) continue;

        static auto getDisplayName = [=](const auto &fileInfo){
            return fileInfo.displayName();
        };

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->modInfo().name() + ":");
        nameItem->setCheckable(true);
        nameItem->setCheckState(Qt::Checked);
        nameItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        auto beforeItem = new QStandardItem();
        if(type == LocalMod::Curseforge)
            beforeItem->setText(getDisplayName(mod->currentCurseforgeFileInfo().value()));
        else if(type == LocalMod::Modrinth)
            beforeItem->setText(getDisplayName(mod->currentModrinthFileInfo().value()));
        beforeItem->setForeground(Qt::red);
        beforeItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        auto afterItem = new QStandardItem();
        if(type == LocalMod::Curseforge)
            afterItem->setText(getDisplayName(mod->updateCurseforgeFileInfo().value()));
        else if(type == LocalMod::Modrinth)
            afterItem->setText(getDisplayName(mod->updateModrinthFileInfo().value()));
        afterItem->setForeground(Qt::green);
        afterItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        auto sourceItem = new QStandardItem();
        sourceItem->setText(type == LocalMod::Curseforge? "Curseforge" : "Modrinth");
        sourceItem->setIcon(QIcon(":/image/curseforge.svg"));
        sourceItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

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
    //TODO: Select update source
    QList<QPair<LocalMod *, LocalMod::ModWebsiteType>> modUpdateList;
    for(int row = 0; row < model_.rowCount(); row++)
        if(model_.item(row)->checkState() == Qt::Checked)
            modUpdateList << QPair(updateList_.at(row), updateList_.at(row)->updateType());
    modPath_->updateMods(modUpdateList);
}

