#include "preferences.h"
#include "ui_preferences.h"

#include <QFileDialog>

#include "config.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    Config config;
    ui->commonPathText->setText(config.getCommonPath());
    ui->versionMatchSelect->setCurrentIndex(config.getVersionMatch());
    ui->autoSearchOnWebsites->setChecked(config.getAutoSearchOnWebsites());
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
    config.setCommonPath(ui->commonPathText->text());
    config.setVersionMatch(ui->versionMatchSelect->currentIndex());
    config.setAutoSearchOnWebsites(ui->autoSearchOnWebsites->isChecked());
    config.setAutoCheckUpdate(ui->autoCheckUpdate->isChecked());
    config.setDeleteOld(ui->deleteOld->isChecked());

    config.setThreadCount(ui->threadCount->value());
    config.setDownloadCount(ui->downloadCount->value());
}


void Preferences::on_commonPathButton_clicked()
{
    auto str = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), ui->commonPathText->text());
    if(str.isEmpty()) return;
    ui->commonPathText->setText(str);
}

void Preferences::on_autoSearchOnWebsites_stateChanged(int arg1)
{
    ui->autoCheckUpdate->setEnabled(arg1 == Qt::Checked);
    ui->autoCheckUpdate->setChecked(arg1 == Qt::Checked);
}

