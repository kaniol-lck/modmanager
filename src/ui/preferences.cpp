#include "preferences.h"
#include "ui_preferences.h"

#include <QFileDialog>

#include "config.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    ui->proxyBox->setVisible(false);

    Config config;
    ui->commonPathText->setText(config.getCommonPath());
    ui->downloadPathText->setText(config.getDownloadPath());
    ui->versionMatchSelect->setCurrentIndex(config.getVersionMatch());
    ui->autoSearchOnWebsites->setChecked(config.getAutoSearchOnWebsites());
    ui->autoCheckUpdate->setChecked(config.getAutoCheckUpdate());
    ui->useCurseforgeUpdate->setChecked(config.getUseCurseforgeUpdate());
    ui->useModrinthUpdate->setChecked(config.getUseModrinthUpdate());
    ui->postUpdate->setCurrentIndex(config.getPostUpdate());

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
    config.setDownloadPath(ui->downloadPathText->text());
    config.setVersionMatch(ui->versionMatchSelect->currentIndex());
    config.setAutoSearchOnWebsites(ui->autoSearchOnWebsites->isChecked());
    config.setAutoCheckUpdate(ui->autoCheckUpdate->isChecked());
    config.setUseCurseforgeUpdate(ui->useCurseforgeUpdate->isChecked());
    config.setUseModrinthUpdate(ui->useModrinthUpdate->isChecked());
    config.setPostUpdate(ui->postUpdate->currentIndex());

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


void Preferences::on_downloadPathButton_clicked()
{
    auto str = QFileDialog::getExistingDirectory(this, tr("Select default mod download directory..."), ui->downloadPathText->text());
    if(str.isEmpty()) return;
    ui->downloadPathText->setText(str);
}

