#include "localmoditemwidget.h"
#include "ui_modentrywidget.h"

#include "localmodinfo.h"

LocalModItemWidget::LocalModItemWidget(QWidget *parent, const LocalModInfo &modInfo) :
    QWidget(parent),
    ui(new Ui::ModEntryWidget)
{
    ui->setupUi(this);
    ui->modIcon->setFixedSize(80, 80);
    ui->modName->setText(modInfo.getName());
    ui->modVersion->setText(modInfo.getVersion());
    ui->modDescription->setText(modInfo.getDescription());

    if(!modInfo.getIconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(modInfo.getIconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}
