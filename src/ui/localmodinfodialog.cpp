#include "localmodinfodialog.h"
#include "ui_localmodinfodialog.h"

#include <QDesktopServices>
#include <QMessageBox>

#include "localfileitemwidget.h"
#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "curseforgemodinfodialog.h"
#include "modrinthmodinfodialog.h"
#include "util/funcutil.h"

LocalModInfoDialog::LocalModInfoDialog(QWidget *parent, LocalMod *mod) :
    QDialog(parent),
    ui(new Ui::LocalModInfoDialog),
    mod_(mod)
{
    ui->setupUi(this);

    //init info
    updateInfo();

    //signals / slots
    connect(mod_, &LocalMod::modInfoUpdated, this, &LocalModInfoDialog::updateInfo);

    //update curseforge
    auto updateCurseforge = [=](bool bl){
        ui->curseforgeButton->setEnabled(bl);
    };

    if(mod->curseforgeMod() != nullptr)
        updateCurseforge(true);
    else{
        connect(mod_, &LocalMod::curseforgeReady, this, updateCurseforge);
    }

    //update modrinth
    auto updateModrinth = [=](bool bl){
        ui->modrinthButton->setEnabled(bl);
    };

    if(mod->modrinthMod() != nullptr)
        updateModrinth(true);
    else{
        connect(mod_, &LocalMod::modrinthReady, this, updateModrinth);
    }
}

LocalModInfoDialog::~LocalModInfoDialog()
{
    delete ui;
}

void LocalModInfoDialog::updateInfo()
{
    setWindowTitle(mod_->modInfo().name() + tr(" - Local"));

    ui->modName->setText(mod_->modInfo().name());
    ui->modVersion->setText(mod_->modInfo().version());
    ui->modAuthors->setText(mod_->modInfo().authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    ui->modDescription->setText(mod_->modInfo().description());

    ui->websiteButton->setEnabled(true);
    ui->sourceButton->setEnabled(true);
    ui->issueButton->setEnabled(true);

    //TODO: generic icon setter
    if(mod_->modInfo().website().isEmpty())
        ui->websiteButton->setEnabled(false);
    else if(mod_->modInfo().website().toString().contains("curseforge.com"))
        ui->websiteButton->setIcon(QIcon(":/image/curseforge.svg"));
    else if(mod_->modInfo().website().toString().contains("modrinth.com"))
        ui->websiteButton->setIcon(QIcon(":/image/modrinth.svg"));

    if(mod_->modInfo().sources().isEmpty())
        ui->sourceButton->setEnabled(false);
    else if(mod_->modInfo().sources().toString().contains("github.com"))
        ui->sourceButton->setIcon(QIcon(":/image/github.svg"));

    if(mod_->modInfo().issues().isEmpty())
        ui->issueButton->setEnabled(false);
    else if(mod_->modInfo().issues().toString().contains("github.com"))
        ui->issueButton->setIcon(QIcon(":/image/github.svg"));

    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    ui->modIcon->setCursor(Qt::ArrowCursor);

    //file info
    auto fileInfo = mod_->modInfo().fileInfo();
    ui->fileBaseNameText->setText(fileInfo.completeBaseName());
    ui->fileSuffixText->setText("." + fileInfo.suffix());
    ui->sizeText->setText(numberConvert(fileInfo.size(), "B"));
    ui->createdTimeText->setText(fileInfo.fileTime(QFile::FileBirthTime).toString());
    ui->modifiedTimeText->setText(fileInfo.fileTime(QFile::FileModificationTime).toString());

    //old mod info list
    if(mod_->oldInfos().isEmpty())
        ui->tabWidget->setTabEnabled(2, false);
    else{
        ui->tabWidget->setTabEnabled(2, true);
        ui->oldModListWidget->clear();
        for(const auto &fileInfo : mod_->oldInfos()){
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 90));
            auto itemWidget = new LocalFileItemWidget(this, mod_, fileInfo);
            ui->oldModListWidget->addItem(listItem);
            ui->oldModListWidget->setItemWidget(listItem, itemWidget);
        }
    }

}

void LocalModInfoDialog::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod, mod_->modInfo().fileInfo().path(), mod_);
    dialog->show();
}

void LocalModInfoDialog::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod, mod_->modInfo().fileInfo().path(), mod_);
    dialog->show();
}


void LocalModInfoDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().website());
}


void LocalModInfoDialog::on_sourceButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().sources());
}


void LocalModInfoDialog::on_issueButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().issues());
}

void LocalModInfoDialog::on_editFileNameButton_clicked()
{
    ui->fileBaseNameText->setEnabled(true);
}


void LocalModInfoDialog::on_fileBaseNameText_editingFinished()
{
    ui->fileBaseNameText->setEnabled(false);
//    if(!mod_->modInfo().rename(ui->fileBaseNameText->text())) {
//        ui->fileBaseNameText->setText(mod_->modInfo().fileInfo().completeBaseName());
//        QMessageBox::information(this, tr("Rename Failed"), tr("Rename failed!"));
//    }
}

