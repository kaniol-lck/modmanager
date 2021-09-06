#include "localmodbrowsersettingsdialog.h"
#include "ui_localmodbrowsersettingsdialog.h"

#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include "gameversion.h"

LocalModBrowserSettingsDialog::LocalModBrowserSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalModBrowserSettingsDialog)
{
    ui->setupUi(this);
    ui->modDirButton->setIcon(QIcon::fromTheme("folder"));
    info_.setLoaderType(ModLoaderType::Any);
    updateVersions();
}

LocalModBrowserSettingsDialog::LocalModBrowserSettingsDialog(QWidget *parent, const LocalModPathInfo &info) :
    LocalModBrowserSettingsDialog(parent)
{
    info_ = info;
    customName = info.showText();
    ui->nameText->setText(info.name());
    ui->modsDirText->setText(info.path());
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(info.loaderType());

    ui->useAutoName->setChecked(info.isAutoName());
}

LocalModBrowserSettingsDialog::~LocalModBrowserSettingsDialog()
{
    delete ui;
}

void LocalModBrowserSettingsDialog::updateVersions()
{
    ui->versionSelect->clear();
    for(const auto &version : qAsConst(GameVersion::versionList))
        ui->versionSelect->addItem(version);
}

void LocalModBrowserSettingsDialog::on_modDirButton_clicked()
{
    auto str = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), ui->modsDirText->text());
    if(str.isEmpty()) return;

    //version
    auto v = GameVersion::deduceFromString(str);
    if(v.has_value())
        ui->versionSelect->setCurrentText(v.value());
    info_.setGameVersion(ui->versionSelect->currentText());

    //path
    do{
        QDir dir(str);
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
    ui->modsDirText->setText(str);
}


void LocalModBrowserSettingsDialog::on_buttonBox_accepted()
{
    emit settingsUpdated(info_);
}


void LocalModBrowserSettingsDialog::on_useAutoName_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        ui->nameText->setEnabled(false);
        updateAutoName();
    }
    else if(arg1 == Qt::Unchecked){
        ui->nameText->setEnabled(true);
        ui->nameText->setText(customName);
    }
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

