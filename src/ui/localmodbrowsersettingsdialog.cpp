#include "localmodbrowsersettingsdialog.h"
#include "ui_localmodbrowsersettingsdialog.h"

#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include "gameversion.h"
#include "config.h"

LocalModBrowserSettingsDialog::LocalModBrowserSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalModBrowserSettingsDialog)
{
    ui->setupUi(this);
    info_.setLoaderType(ModLoaderType::Any);

    for(const auto &type : ModLoaderType::local)
        ui->loaderSelect->addItem(ModLoaderType::toString(type));

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::mojangVersionListUpdated, this, &LocalModBrowserSettingsDialog::updateVersionList);
}

LocalModBrowserSettingsDialog::LocalModBrowserSettingsDialog(QWidget *parent, const LocalModPathInfo &info) :
    LocalModBrowserSettingsDialog(parent)
{
    info_ = info;
    customName = info.name();
    ui->nameText->setText(info.displayName());
    ui->modsDirText->setText(info.path());
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::local.indexOf(info.loaderType()));

    ui->useAutoName->setChecked(info.isAutoName());
    on_useAutoName_toggled(info.isAutoName());
}

LocalModBrowserSettingsDialog::~LocalModBrowserSettingsDialog()
{
    delete ui;
}

void LocalModBrowserSettingsDialog::updateVersionList()
{
    ui->versionSelect->clear();
    for(const auto &version : GameVersion::mojangVersionList())
        ui->versionSelect->addItem(version);
}

void LocalModBrowserSettingsDialog::on_modDirButton_clicked()
{
    auto str = ui->modsDirText->text();
    if(str.isEmpty()) str = Config().getCommonPath();
    auto resultStr = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), str);
    if(resultStr.isEmpty()) return;

    //version
    auto v = GameVersion::deduceFromString(resultStr);
    if(v.has_value())
        ui->versionSelect->setCurrentText(v.value());
    info_.setGameVersion(ui->versionSelect->currentText());

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
    ui->modsDirText->setText(resultStr);
}


void LocalModBrowserSettingsDialog::on_buttonBox_accepted()
{
    emit settingsUpdated(info_);
}

void LocalModBrowserSettingsDialog::on_loaderSelect_currentIndexChanged(const QString &arg1)
{
    info_.setLoaderType(ModLoaderType::fromString(arg1));
    updateAutoName();
}

void LocalModBrowserSettingsDialog::updateAutoName()
{
    if(!ui->useAutoName->isChecked()) return;
    info_.setName(info_.autoName());
    ui->nameText->setText(info_.autoName());
}

void LocalModBrowserSettingsDialog::on_versionSelect_currentIndexChanged(const QString &arg1)
{
    info_.setGameVersion(arg1);
    updateAutoName();
}

void LocalModBrowserSettingsDialog::on_nameText_textEdited(const QString &arg1)
{
    customName = arg1;
    info_.setName(arg1);
}


void LocalModBrowserSettingsDialog::on_useAutoName_toggled(bool checked)
{
    info_.setIsAutoName(checked);
    ui->nameText->setEnabled(!checked);
    if(checked)
        updateAutoName();
    else
        ui->nameText->setText(customName);
}

