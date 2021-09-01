#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

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

    for(const auto &mod : list){
        //skip if no update
        if(!mod->getUpdateCurseforgeFileInfo().has_value()) continue;

        updateList << mod;

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->getModInfo().getName() + ":");
        nameItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        nameItem->setCheckable(true);
        nameItem->setCheckState(Qt::Checked);

        auto beforeItem = new QStandardItem();
        beforeItem->setText(mod->getCurrentCurseforgeFileInfo().value().getDisplayName());
        beforeItem->setForeground(Qt::red);

        auto afterItem = new QStandardItem();
        afterItem->setText(mod->getUpdateCurseforgeFileInfo().value().getDisplayName());
        model.appendRow({nameItem, beforeItem, afterItem});
        afterItem->setForeground(Qt::green);
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
            updateList.at(row)->update();
}

