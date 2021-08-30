#include "preferences.h"
#include "ui_preferences.h"

#include "config.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    Config config;
    ui->autoCheckUpdate->setChecked(config.getAutoCheckUpdate());
    ui->deleteOld->setChecked(config.getDeleteOld());
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::on_Preferences_accepted()
{
    Config config;
    config.setAutoCheckUpdate(ui->autoCheckUpdate->isChecked());
    config.setDeleteOld(ui->deleteOld->isChecked());
}

