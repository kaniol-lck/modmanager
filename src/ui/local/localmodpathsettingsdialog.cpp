#include "localmodpathsettingsdialog.h"
#include "ui_localmodpathsettingsdialog.h"

#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include "gameversion.h"
#include "config.hpp"

LocalModPathSettingsDialog::LocalModPathSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalModPathSettingsDialog)
{
    ui->setupUi(this);
    ui->loaderSelect->clear();
    for(auto &&loaderType: ModLoaderType::local)
        ui->loaderSelect->addItem(ModLoaderType::icon(loaderType), ModLoaderType::toString(loaderType));

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::mojangVersionListUpdated, this, &LocalModPathSettingsDialog::updateVersionList);
}

LocalModPathSettingsDialog::LocalModPathSettingsDialog(QWidget *parent, const LocalModPathInfo &info) :
    LocalModPathSettingsDialog(parent)
{
    info_ = info;
    customName = info.name();
    ui->nameText->setText(info.displayName());
    ui->modsDirText->setText(info.path());
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->clear();
    for(auto &&loaderType: ModLoaderType::local)
        ui->loaderSelect->addItem(ModLoaderType::icon(loaderType), ModLoaderType::toString(loaderType));
    ui->loaderSelect->setCurrentIndex(ModLoaderType::local.indexOf(info.loaderType()));

    ui->useAutoName->setChecked(info.isAutoName());
    on_useAutoName_toggled(info.isAutoName());
}

LocalModPathSettingsDialog::~LocalModPathSettingsDialog()
{
    delete ui;
}

void LocalModPathSettingsDialog::updateVersionList()
{
    ui->versionSelect->clear();
    ui->versionSelect->addItem(tr("Any"));
    for(auto &&version : GameVersion::mojangReleaseVersionList())
        ui->versionSelect->addItem(version);
}

void LocalModPathSettingsDialog::on_modDirButton_clicked()
{
    auto str = ui->modsDirText->text();
    if(str.isEmpty()) str = Config().getCommonPath();
    auto resultStr = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), str);
    if(resultStr.isEmpty()) return;

    //version
    auto version = GameVersion::deduceFromString(resultStr);
    if(version != GameVersion::Any)
        ui->versionSelect->setCurrentText(version);

    //path
    do{
        QDir dir(resultStr);
        QString path;
        if(dir.dirName() == "mods")
            path = dir.absolutePath();
        else if(dir.dirName() == ".minecraft"){
            if(!dir.cd("mods")) break;
            path = dir.absolutePath();
        } else{
            if(!dir.cd(".minecraft")) break;
            if(!dir.cd("mods")) break;
            path = dir.absolutePath();
        }
        ui->modsDirText->setText(path);
        info_.setPath(path);
        updateAutoName();
        return;
    } while(false);
    info_.setPath(resultStr);
    ui->modsDirText->setText(resultStr);
}

void LocalModPathSettingsDialog::on_buttonBox_accepted()
{
    emit settingsUpdated(info_);
}

void LocalModPathSettingsDialog::updateAutoName()
{
    if(!ui->useAutoName->isChecked()) return;
    info_.setName(info_.autoName());
    ui->nameText->setText(info_.autoName());
}

void LocalModPathSettingsDialog::on_versionSelect_currentIndexChanged(int index)
{
    if(index < 0) return;
    info_.setGameVersion(index? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any);
    updateAutoName();
}

void LocalModPathSettingsDialog::on_nameText_textEdited(const QString &arg1)
{
    customName = arg1;
    info_.setName(arg1);
}


void LocalModPathSettingsDialog::on_useAutoName_toggled(bool checked)
{
    info_.setIsAutoName(checked);
    ui->nameText->setEnabled(!checked);
    if(checked)
        updateAutoName();
    else
        ui->nameText->setText(customName);
}

void LocalModPathSettingsDialog::on_loaderSelect_currentIndexChanged(int index)
{
    if(index < 0) return;
    info_.setLoaderType(ModLoaderType::local.at(index));
    updateAutoName();
}

