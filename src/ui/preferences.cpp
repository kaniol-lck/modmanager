#include "preferences.h"
#include "ui_preferences.h"

#include <QFileDialog>

#include "config.hpp"
#include "qss/stylesheets.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
#if !defined (DE_KDE) && !defined (Q_OS_WIN)
    ui->enableBlurBehind->setVisible(false);
#endif
#if !defined (Q_OS_WIN)
    ui->useFramelessWindow->setVisible(false);
#endif
    for(auto it = styleSheets().cbegin(); it != styleSheets().cend(); it++){
        ui->customStyle->addItem(it.value());
        ui->customStyle->setItemData(ui->customStyle->count() - 1, it.key());
    }

    ui->language->addItem(tr("Follow System"));
    for(auto &&str : { "en_US", "zh_CN", "zh_TW" }){
        QLocale locale(str);
        ui->language->addItem(locale.nativeLanguageName(), str);
    }

    Config config;
    //General
    ui->smoothScroll->setChecked(config.getSmoothScroll());
    ui->scrollSpeed->setValue(config.getScrollSpeed());
    ui->scrollAcceleration->setValue(config.getScrollAcceleration());
    ui->scrollFriction->setValue(config.getScrollFriction());
    ui->showModAuthors->setChecked(config.getShowModAuthors());
    ui->showModDateTime->setChecked(config.getShowModDateTime());
    ui->showModCategory->setChecked(config.getShowModCategory());
    ui->showModLoaderType->setChecked(config.getShowModLoaderType());
    ui->showModReleaseType->setChecked(config.getShowModReleaseType());
    ui->showModGameVersion->setChecked(config.getShowModGameVersion());
    ui->enableBlurBehind->setChecked(config.getEnableBlurBehind());
    ui->useFramelessWindow->setChecked(config.getUseFramelessWindow());
    on_useFramelessWindow_toggled(ui->useFramelessWindow->isChecked());
    ui->customStyle->setCurrentText(styleSheets().value(config.getCustomStyle()));
    ui->autoTranslate->setChecked(config.getAutoTranslate());
    ui->useSystemIconTheme->setChecked(config.getUseSystemIconTheme());
    ui->autoCheckModManagerUpdate->setCurrentIndex(config.getAutoCheckModManagerUpdate());
    for(int i = 0; i < ui->language->count(); i++){
        if(ui->language->itemData(i).toString() == config.getLanguage()){
            ui->language->setCurrentIndex(i);
            break;
        }
    }

    //Explore
    ui->downloadPathText->setText(config.getDownloadPath());
    ui->searchResultCount->setValue(config.getSearchResultCount());
    ui->searchModsOnStartup->setChecked(config.getSearchModsOnStartup());
    ui->autoFetchModrinthFileList->setChecked(config.getAutoFetchModrinthFileList());
    ui->optifineVersionSource->setCurrentIndex(config.getOptifineSource());
    ui->showCatVerInToolBar->setChecked(config.getShowCatVerInToolBar());
    ui->showCurseforge->setChecked(config.getShowCurseforge());
    ui->showModrinth->setChecked(config.getShowModrinth());
    ui->showOptiFine->setChecked(config.getShowOptiFine());
    ui->showReplayMod->setChecked(config.getShowReplayMod());

    //Local
    ui->commonPathText->setText(config.getCommonPath());
    ui->rightClickTagMenu->setCurrentIndex(config.getRightClickTagMenu());
    ui->loadModsOnStartup->setChecked(config.getLoadModsOnStartup());
    ui->starredAtTop->setChecked(config.getStarredAtTop());
    ui->disabedAtBottom->setChecked(config.getDisabedAtBottom());
    ui->versionMatchSelect->setCurrentIndex(config.getVersionMatch());
    ui->loaderMatchSelect->setCurrentIndex(config.getLoaderMatch());
    ui->updateCheckIntervalSelect->setCurrentIndex(config.getUpdateCheckInterval());
    ui->useCurseforgeUpdate->setChecked(config.getUseCurseforgeUpdate());
    ui->useModrinthUpdate->setChecked(config.getUseModrinthUpdate());
    ui->useBetaUpdate->setChecked(config.getUseBetaUpdate());
    ui->useAlphaUpdate->setChecked(config.getUseAlphaUpdate());
    ui->postUpdate->setCurrentIndex(config.getPostUpdate());

    //Network
    ui->aria2timeout->setValue(config.getAria2timeout());
    ui->aria2maxTries->setValue(config.getAria2timeout());
    ui->aria2maxConcurrentDownloads->setValue(config.getAria2maxConcurrentDownloads());
    ui->networkRequestTimeout->setValue(config.getNetworkRequestTimeout() / 1000);

    ui->proxyType->setCurrentIndex(config.getProxyType());
    ui->proxyHostName->setText(config.getProxyHostName());
    ui->proxyPort->setValue(config.getProxyPort());
    ui->proxyUser->setText(config.getProxyUser());
    ui->proxyPassword->setText(config.getProxyPassword());
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
    config.setShowModAuthors(ui->showModAuthors->isChecked());
    config.setShowModDateTime(ui->showModDateTime->isChecked());
    config.setShowModCategory(ui->showModCategory->isChecked());
    config.setShowModLoaderType(ui->showModLoaderType->isChecked());
    config.setShowModReleaseType(ui->showModReleaseType->isChecked());
    config.setShowModGameVersion(ui->showModGameVersion->isChecked());
    config.setEnableBlurBehind(ui->enableBlurBehind->isChecked());
    config.setUseFramelessWindow(ui->useFramelessWindow->isChecked());
    config.setCustomStyle(ui->customStyle->currentData().toString());
    config.setAutoTranslate(ui->autoTranslate->isChecked());
    config.setUseSystemIconTheme(ui->useSystemIconTheme->isChecked());
    config.setAutoCheckModManagerUpdate(ui->autoCheckModManagerUpdate->currentIndex());
    config.setLanguage(ui->language->currentData().toString());

    //Explore
    config.setDownloadPath(ui->downloadPathText->text());
    config.setSearchResultCount(ui->searchResultCount->value());
    config.setSearchModsOnStartup(ui->searchModsOnStartup->isChecked());
    config.setAutoFetchModrinthFileList(ui->autoFetchModrinthFileList->isChecked());
    config.setOptifineSource(ui->optifineVersionSource->currentIndex());
    config.setShowCatVerInToolBar(ui->showCatVerInToolBar->isChecked());
    config.setShowCurseforge(ui->showCurseforge->isChecked());
    config.setShowModrinth(ui->showModrinth->isChecked());
    config.setShowOptiFine(ui->showOptiFine->isChecked());
    config.setShowReplayMod(ui->showReplayMod->isChecked());

    //Local
    config.setCommonPath(ui->commonPathText->text());
    config.setRightClickTagMenu(ui->rightClickTagMenu->currentIndex());
    config.setLoadModsOnStartup(ui->loadModsOnStartup->isChecked());
    config.setStarredAtTop(ui->starredAtTop->isChecked());
    config.setDisabedAtBottom(ui->disabedAtBottom->isChecked());
    config.setVersionMatch(ui->versionMatchSelect->currentIndex());
    config.setLoaderMatch(ui->loaderMatchSelect->currentIndex());
    config.setUpdateCheckInterval(ui->updateCheckIntervalSelect->currentIndex());
    config.setUseCurseforgeUpdate(ui->useCurseforgeUpdate->isChecked());
    config.setUseModrinthUpdate(ui->useModrinthUpdate->isChecked());
    config.setUseBetaUpdate(ui->useBetaUpdate->isChecked());
    config.setUseAlphaUpdate(ui->useAlphaUpdate->isChecked());
    config.setPostUpdate(ui->postUpdate->currentIndex());

    config.setAria2timeout(ui->aria2timeout->value());
    config.setAria2maxTries(ui->aria2maxTries->value());
    config.setAria2maxConcurrentDownloads(ui->aria2maxConcurrentDownloads->value());
    config.setNetworkRequestTimeout(ui->networkRequestTimeout->value() * 1000);

    config.setProxyType(ui->proxyType->currentIndex());
    config.setProxyHostName(ui->proxyHostName->text());
    config.setProxyPort(ui->proxyPort->value());
    config.setProxyUser(ui->proxyUser->text());
    config.setProxyPassword(ui->proxyPassword->text());
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

void Preferences::on_proxyType_currentIndexChanged(int index)
{
    //No Proxy
    bool noProxy = index == 2;
    ui->proxyHostName->setEnabled(!noProxy);
    ui->proxyPort->setEnabled(!noProxy);
    ui->proxyUser->setEnabled(!noProxy);
    ui->proxyPassword->setEnabled(!noProxy);
}


void Preferences::on_useFramelessWindow_toggled(bool checked[[maybe_unused]])
{
#ifdef Q_OS_WIN
    if(!checked)
        ui->enableBlurBehind->setChecked(false);
    ui->enableBlurBehind->setEnabled(checked);
#endif
}
