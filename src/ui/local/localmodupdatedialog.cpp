#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"

#include <QComboBox>

LocalModUpdateDialog::LocalModUpdateDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::LocalModUpdateDialog),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->updateTreeView->setModel(&model_);
    ui->updateTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);

    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(BeforeColumn, new QStandardItem(tr("Old Version")));
    model_.setHorizontalHeaderItem(AfterColumn, new QStandardItem(tr("New Version")));
    model_.setHorizontalHeaderItem(SourceColumn, new QStandardItem(tr("Update Source")));

    ui->updateTreeView->header()->setSectionResizeMode(NameColumn, QHeaderView::Fixed);
    ui->updateTreeView->setColumnWidth(NameColumn, 250);
    ui->updateTreeView->header()->setSectionResizeMode(SourceColumn, QHeaderView::Fixed);
    ui->updateTreeView->setColumnWidth(SourceColumn, 140);

    for(auto &&map : modPath_->modMaps())
        for(const auto &mod : map){
            auto enabled = !mod->isDisabled();

            auto type = mod->defaultUpdateType();
            auto types = mod->updateTypes();
            if(type == ModWebsiteType::None) continue;
            auto names = mod->updateNames(type);
            auto infos = mod->updateInfos(type);

            auto nameItem = new QStandardItem();
            nameItem->setText(mod->commonInfo()->name());
            nameItem->setCheckable(enabled);
            nameItem->setCheckState(enabled? Qt::Checked : Qt::Unchecked);
            nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            nameItem->setEnabled(enabled);

            auto beforeItem = new QStandardItem();
            beforeItem->setText(names.first);
            if(enabled) beforeItem->setForeground(Qt::darkRed);
            beforeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            beforeItem->setEnabled(enabled);
            beforeItem->setToolTip(infos.first);

            auto afterItem = new QStandardItem();
            afterItem->setText(names.second);
            if(enabled) afterItem->setForeground(Qt::darkGreen);
            afterItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            afterItem->setEnabled(enabled);
            afterItem->setToolTip(infos.second);

            auto sourceItem = new QStandardItem();
            sourceItem->setData(type);
            if(types.size() == 1){
                sourceItem->setText(ModWebsite::toString(type));
                sourceItem->setIcon(ModWebsite::icon(type));
                sourceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }
            sourceItem->setEnabled(enabled);

            model_.appendRow({nameItem, beforeItem, afterItem, sourceItem});
            modList_ << mod;

            if(types.size() > 1){
                auto comboBox = new QComboBox(this);
                ui->updateTreeView->setIndexWidget(model_.indexFromItem(sourceItem), comboBox);
                for(auto &&type : types){
                    comboBox->addItem(ModWebsite::icon(type), ModWebsite::toString(type));
                }
                connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
                    auto type = types.at(index);
                    sourceItem->setData(type);
                    auto names = mod->updateNames(type);
                    beforeItem->setText(names.first);
                    afterItem->setText(names.second);
                });
            }
        }
}

LocalModUpdateDialog::~LocalModUpdateDialog()
{
    delete ui;
}

void LocalModUpdateDialog::on_LocalModUpdateDialog_accepted()
{
    QList<QPair<LocalMod *, ModWebsiteType>> modUpdateList;
    for(int row = 0; row < model_.rowCount(); row++)
        if(model_.item(row)->checkState() == Qt::Checked)
            modUpdateList << QPair<LocalMod *, ModWebsiteType>(modList_.at(row), static_cast<ModWebsiteType>(model_.item(row, SourceColumn)->data().toInt()));
    modPath_->updateMods(modUpdateList);
}
