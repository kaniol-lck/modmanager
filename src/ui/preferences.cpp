#include "preferences.h"
#include "ui_preferences.h"

#include <QFileDialog>

#include "config.hpp"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    ui->proxyBox->setVisible(false);
    //TODO
    ui->label_11->setVisible(false);
    ui->optifineVersionSource->setVisible(false);

    Config config;
    //General
    ui->smoothScroll->setChecked(config.getSmoothScroll());
    ui->scrollSpeed->setValue(config.getScrollSpeed());
    ui->scrollAcceleration->setValue(config.getScrollAcceleration());
    ui->scrollFriction->setValue(config.getScrollFriction());

    //Explore
    ui->downloadPathText->setText(config.getDownloadPath());
    ui->searchResultCount->setValue(config.getSearchResultCount());
    ui->optifineVersionSource->setCurrentIndex(config.getOptifineSource());
    ui->showModrinthSnapshot->setChecked(config.getShowModrinthSnapshot());

    //Local
    ui->commonPathText->setText(config.getCommonPath());
    ui->rightClickTagMenu->setCurrentIndex(config.getRightClickTagMenu());
    ui->starredAtTop->setChecked(config.getStarredAtTop());
    ui->disabedAtBottom->setChecked(config.getDisabedAtBottom());
    ui->versionMatchSelect->setCurrentIndex(config.getVersionMatch());
    ui->loaderMatchSelect->setCurrentIndex(config.getLoaderMatch());
    ui->updateCheckIntervalSelect->setCurrentIndex(config.getUpdateCheckInterval());
    ui->useCurseforgeUpdate->setChecked(config.getUseCurseforgeUpdate());
    ui->useModrinthUpdate->setChecked(config.getUseModrinthUpdate());
    ui->postUpdate->setCurrentIndex(config.getPostUpdate());

    //Network
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
    //General
    config.setSmoothScroll(ui->smoothScroll->isChecked());
    config.setScrollSpeed(ui->scrollSpeed->value());
    config.setScrollAcceleration(ui->scrollAcceleration->value());
    config.setScrollFriction(ui->scrollFriction->value());

    //Explore
    config.setDownloadPath(ui->downloadPathText->text());
    config.setSearchResultCount(ui->searchResultCount->value());
    config.setOptifineSource(ui->optifineVersionSource->currentIndex());
    config.setShowModrinthSnapshot(ui->showModrinthSnapshot->isChecked());

    //Local
    config.setCommonPath(ui->commonPathText->text());
    config.setRightClickTagMenu(ui->rightClickTagMenu->currentIndex());
    config.setStarredAtTop(ui->starredAtTop->isChecked());
    config.setDisabedAtBottom(ui->disabedAtBottom->isChecked());
    config.setVersionMatch(ui->versionMatchSelect->currentIndex());
    config.setLoaderMatch(ui->loaderMatchSelect->currentIndex());
    config.setUpdateCheckInterval(ui->updateCheckIntervalSelect->currentIndex());
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

void Preferences::on_downloadPathButton_clicked()
{
    auto str = QFileDialog::getExistingDirectory(this, tr("Select default mod download directory..."), ui->downloadPathText->text());
    if(str.isEmpty()) return;
    ui->downloadPathText->setText(str);
}

