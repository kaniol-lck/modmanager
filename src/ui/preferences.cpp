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

    ui->threadCount->setValue(config.getThreadCount());
    ui->downloadCount->setValue(config.getDownloadCount());
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

    config.setThreadCount(ui->threadCount->value());
    config.setDownloadCount(ui->downloadCount->value());
}

