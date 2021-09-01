#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include <tuple>

LocalModUpdateDialog::LocalModUpdateDialog(QWidget *parent, const QList<LocalMod *> &list) :
    QDialog(parent),
    ui(new Ui::LocalModUpdateDialog)
{
    ui->setupUi(this);
    ui->updateTableView->setModel(&model);
    ui->updateTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    model.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Name")));
    model.setHorizontalHeaderItem(BeforeColumn, new QStandardItem(tr("Before")));
    model.setHorizontalHeaderItem(AfterColumn, new QStandardItem(tr("After")));
    model.setHorizontalHeaderItem(SourceColumn, new QStandardItem(tr("Source")));

    for(const auto &mod : list){
        auto append = [=](const auto &currentFileInfo, const auto &updateFileInfo, const QString &type){
            auto nameItem = new QStandardItem();
            nameItem->setText(mod->getModInfo().getName() + ":");
            nameItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            nameItem->setCheckable(true);
            nameItem->setCheckState(Qt::Checked);

            auto beforeItem = new QStandardItem();
            beforeItem->setText(currentFileInfo.getDisplayName());
            beforeItem->setForeground(Qt::red);

            auto afterItem = new QStandardItem();
            afterItem->setText(updateFileInfo.getDisplayName());
            afterItem->setForeground(Qt::green);

            auto sourceItem = new QStandardItem();
            sourceItem->setText(type);

            model.appendRow({nameItem, beforeItem, afterItem, sourceItem});
        };

        auto type = mod->updateType();
        if(type == LocalMod::ModWebsiteType::Curseforge)
            append(mod->getCurrentCurseforgeFileInfo().value(), mod->getUpdateCurseforgeFileInfo().value(), "Curseforge");
        else if(type == LocalMod::ModWebsiteType::Modrinth)
            append(mod->getCurrentModrinthFileInfo().value(), mod->getUpdateModrinthFileInfo().value(), "Modrinth");
    }
}

LocalModUpdateDialog::~LocalModUpdateDialog()
{
    delete ui;
}


void LocalModUpdateDialog::on_LocalModUpdateDialog_accepted()
{
    for(int row = 0; row < model.rowCount(); row++)
        if(model.item(row)->checkState() == Qt::Checked)
            updateList.at(row)->update(updateList.at(row)->updateType());
}

