#include "optifinemoditemwidget.h"
#include "ui_optifinemoditemwidget.h"

#include "optifine/optifinemod.h"

OptifineModItemWidget::OptifineModItemWidget(QWidget *parent, OptifineMod *mod) :
    QWidget(parent),
    ui(new Ui::OptifineModItemWidget),
    mod_(mod)
{
    ui->setupUi(this);
    ui->downloadSpeedText->setVisible(false);
    ui->downloadProgress->setVisible(false);

    ui->displayNameText->setText(mod_->modInfo().name());
    if(mod_->modInfo().downloadUrl().isEmpty()){
        ui->downloadButton->setDisabled(true);
        connect(mod_, &OptifineMod::downloadUrlReady, this, [=]{
            ui->downloadButton->setEnabled(true);
        });
    }
}

OptifineModItemWidget::~OptifineModItemWidget()
{
    delete ui;
}

void OptifineModItemWidget::on_downloadButton_clicked()
{
    //download
}

OptifineMod *OptifineModItemWidget::mod() const
{
    return mod_;
}

