#include "localmodbrowsersettingsdialog.h"
#include "ui_localmodbrowsersettingsdialog.h"

#include <QFileDialog>
#include <QDir>

#include "gameversion.h"

LocalModBrowserSettingsDialog::LocalModBrowserSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalModBrowserSettingsDialog)
{
    ui->setupUi(this);

    for(const auto &version : qAsConst(GameVersion::versionList))
        ui->versionSelect->addItem(version);
}

LocalModBrowserSettingsDialog::LocalModBrowserSettingsDialog(QWidget *parent, const ModDirInfo &info) :
    LocalModBrowserSettingsDialog(parent)
{
    ui->modsDirText->setText(info.getModDir().absolutePath());
    ui->versionSelect->setCurrentText(info.getGameVersion());
    ui->loaderSelect->setCurrentText(info.getLoaderType());
}

LocalModBrowserSettingsDialog::~LocalModBrowserSettingsDialog()
{
    delete ui;
}

void LocalModBrowserSettingsDialog::on_modDirButton_clicked()
{
    auto str = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), ui->modsDirText->text());
    if(str.isEmpty()) return;

    //version
    auto v = GameVersion::deduceFromString(str);
    if(v.has_value())
        ui->versionSelect->setCurrentText(v.value());

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
        return;
    } while(false);
    ui->modsDirText->setText(str);
}


void LocalModBrowserSettingsDialog::on_buttonBox_accepted()
{
    emit settingsUpdated(ModDirInfo(
                             QDir(ui->modsDirText->text()),
                             ui->versionSelect->currentText(),
                             ui->loaderSelect->currentText()));
}

