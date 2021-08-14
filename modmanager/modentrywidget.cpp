#include "modentrywidget.h"
#include "ui_modentrywidget.h"

#include "modinfo.h"

ModEntryWidget::ModEntryWidget(QWidget *parent, const ModInfo &modInfo) :
    QWidget(parent),
    ui(new Ui::ModEntryWidget)
{
    ui->setupUi(this);
    ui->modName->setText(modInfo.getName());
    ui->modVersion->setText(modInfo.getVersion());
    ui->modDescription->setText(modInfo.getDescription());

    if(!modInfo.getIconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(modInfo.getIconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }
    ui->modIcon->setFixedSize(80, 80);
}

ModEntryWidget::~ModEntryWidget()
{
    delete ui;
}
