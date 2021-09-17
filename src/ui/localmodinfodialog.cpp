#include "localmodinfodialog.h"
#include "ui_localmodinfodialog.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QDebug>

#include "localfileitemwidget.h"
#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "curseforgemodinfodialog.h"
#include "modrinthmodinfodialog.h"
#include "util/funcutil.h"

LocalModInfoDialog::LocalModInfoDialog(QWidget *parent, LocalModFile *file, LocalMod *mod) :
    QDialog(parent),
    ui(new Ui::LocalModInfoDialog),
    file_(file),
    mod_(mod)
{
    ui->setupUi(this);

    //init info
    updateInfo();

    if(mod_){
        //signals / slots
        connect(mod_, &LocalMod::modFileUpdated, this, &LocalModInfoDialog::updateInfo);

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
    } else{
        ui->curseforgeButton->setVisible(false);
        ui->modrinthButton->setVisible(false);
        //remove old tab
        ui->tabWidget->removeTab(2);
    }
}

LocalModInfoDialog::~LocalModInfoDialog()
{
    delete ui;
}

void LocalModInfoDialog::updateInfo()
{
    if(mod_) file_ = mod_->modFile();

    auto modInfo = file_->commonInfo();

    if(mod_)
        setWindowTitle(modInfo->name() + tr(" - Local"));
    else
        setWindowTitle(modInfo->name() + tr(" - Local Old"));

    ui->modName->setText(modInfo->name());
    ui->modVersion->setText(modInfo->version());
    ui->modAuthors->setText(modInfo->authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    ui->modDescription->setText(modInfo->description());

    ui->websiteButton->setEnabled(true);
    ui->sourceButton->setEnabled(true);
    ui->issueButton->setEnabled(true);

    //TODO: generic icon setter
    if(modInfo->homepage().isEmpty())
        ui->websiteButton->setEnabled(false);
    else if(modInfo->homepage().toString().contains("curseforge.com"))
        ui->websiteButton->setIcon(QIcon(":/image/curseforge.svg"));
    else if(modInfo->homepage().toString().contains("modrinth.com"))
        ui->websiteButton->setIcon(QIcon(":/image/modrinth.svg"));

    if(modInfo->sources().isEmpty())
        ui->sourceButton->setEnabled(false);
    else if(modInfo->sources().toString().contains("github.com"))
        ui->sourceButton->setIcon(QIcon(":/image/github.svg"));

    if(modInfo->issues().isEmpty())
        ui->issueButton->setEnabled(false);
    else if(modInfo->issues().toString().contains("github.com"))
        ui->issueButton->setIcon(QIcon(":/image/github.svg"));

    QPixmap pixelmap;
    pixelmap.loadFromData(modInfo->iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);

    //file info
    auto fileInfo = file_->fileInfo();
    auto [ baseName, suffix ] = file_->baseNameFullSuffix();
    ui->fileBaseNameText->setText(baseName);
    ui->fileSuffixText->setText(suffix);
    ui->sizeText->setText(numberConvert(fileInfo.size(), "B"));
    ui->createdTimeText->setText(fileInfo.fileTime(QFile::FileBirthTime).toString());
    ui->modifiedTimeText->setText(fileInfo.fileTime(QFile::FileModificationTime).toString());

    if(mod_){
        //old mod info list
        if(mod_->oldFiles().isEmpty())
            ui->tabWidget->setTabEnabled(2, false);
        else{
            ui->tabWidget->setTabEnabled(2, true);
            ui->oldModListWidget->clear();
            for(const auto &file : mod_->oldFiles()){
                auto *listItem = new QListWidgetItem();
                listItem->setSizeHint(QSize(500, 90));
                auto itemWidget = new LocalFileItemWidget(this, mod_, file);
                ui->oldModListWidget->addItem(listItem);
                ui->oldModListWidget->setItemWidget(listItem, itemWidget);
            }
        }
    }
}

void LocalModInfoDialog::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod, file_->fileInfo().path(), mod_);
    dialog->show();
}

void LocalModInfoDialog::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod, file_->fileInfo().path(), mod_);
    dialog->show();
}

void LocalModInfoDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(file_->commonInfo()->homepage());
}


void LocalModInfoDialog::on_sourceButton_clicked()
{
    QDesktopServices::openUrl(file_->commonInfo()->sources());
}


void LocalModInfoDialog::on_issueButton_clicked()
{
    QDesktopServices::openUrl(file_->commonInfo()->issues());
}

void LocalModInfoDialog::on_editFileNameButton_clicked()
{
    ui->fileBaseNameText->setEnabled(true);
    ui->fileBaseNameText->setFocus();
}

void LocalModInfoDialog::on_fileBaseNameText_editingFinished()
{
    QString baseName;
    if(isRenaming) return;
    ui->fileBaseNameText->setEnabled(false);
    baseName = std::get<0>(file_->baseNameFullSuffix());
    if(ui->fileBaseNameText->text() == baseName)
        return;
    isRenaming = true;
    if(!file_->rename(ui->fileBaseNameText->text())) {
        ui->fileBaseNameText->setText(baseName);
        QMessageBox::information(this, tr("Rename Failed"), tr("Rename failed!"));
    }
    isRenaming = false;
}

void LocalModInfoDialog::on_oldModListWidget_doubleClicked(const QModelIndex &index)
{
    auto file = mod_->oldFiles().at(index.row());
    auto dialog = new LocalModInfoDialog(this, file);
    dialog->show();
}

