#include "localmodcheckdialog.h"
#include "ui_localmodcheckdialog.h"

#include <QTreeWidgetItem>
#include <QDebug>

#include "local/localmodpath.h"
#include "localmoditemwidget.h"

LocalModCheckDialog::LocalModCheckDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::LocalModCheckDialog),
    modPath_(modPath),
    dependsItem_(new QTreeWidgetItem({tr("Depends")})),
    conflictsItem_(new QTreeWidgetItem({tr("Conflicts")})),
    breaksItem_(new QTreeWidgetItem({tr("Breaks")}))
{
    ui->setupUi(this);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);

    //setup tree widget
    for (const auto &item : { dependsItem_, conflictsItem_, breaksItem_ }){
        item->setForeground(0, Qt::gray);
        item->setFlags(item->flags().setFlag(Qt::ItemIsSelectable, false));
        ui->treeWidget->addTopLevelItem(item);
    }
    ui->treeWidget->expandAll();
    ui->treeWidget->setColumnWidth(0, 700);

    //fabric
    if(modPath_->info().loaderType() == ModLoaderType::Fabric){
        //depends
        for(const auto &[fabricMod, modid, version, missingMod] : modPath_->checkFabricDepends()){
            QString str;
            if(missingMod.has_value())
                str += "Missing:\n" + modid + " " + version;
            else
                str += "MisMatch:\n" + modid + " " + version;

            auto item = new QTreeWidgetItem(dependsItem_, { "", str });
            item->setSizeHint(0, QSize(0, 108));

            auto localMod = modPath_->modMap().value(fabricMod.mainId());
            auto widget = new LocalModItemWidget(this, localMod);
            ui->treeWidget->setItemWidget(item, 0, widget);

            dependsItem_->addChild(item);
        }
        modPath_->checkFabricConflicts();
        modPath_->checkFabricBreaks();
    }
}

LocalModCheckDialog::~LocalModCheckDialog()
{
    delete ui;
}
