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

LocalModInfoDialog::LocalModInfoDialog(QWidget *parent, const LocalModInfo &modInfo, LocalMod *mod) :
    QDialog(parent),
    ui(new Ui::LocalModInfoDialog),
    modInfo_(modInfo),
    mod_(mod)
{
    ui->setupUi(this);

    //init info
    updateInfo();

    if(mod_){
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
    if(mod_) modInfo_ = mod_->modInfo();

    setWindowTitle(modInfo_.name() + tr(" - Local"));

    ui->modName->setText(modInfo_.name());
    ui->modVersion->setText(modInfo_.version());
    ui->modAuthors->setText(modInfo_.authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    ui->modDescription->setText(modInfo_.description());

    ui->websiteButton->setEnabled(true);
    ui->sourceButton->setEnabled(true);
    ui->issueButton->setEnabled(true);

    //TODO: generic icon setter
    if(modInfo_.website().isEmpty())
        ui->websiteButton->setEnabled(false);
    else if(modInfo_.website().toString().contains("curseforge.com"))
        ui->websiteButton->setIcon(QIcon(":/image/curseforge.svg"));
    else if(modInfo_.website().toString().contains("modrinth.com"))
        ui->websiteButton->setIcon(QIcon(":/image/modrinth.svg"));

    if(modInfo_.sources().isEmpty())
        ui->sourceButton->setEnabled(false);
    else if(modInfo_.sources().toString().contains("github.com"))
        ui->sourceButton->setIcon(QIcon(":/image/github.svg"));

    if(modInfo_.issues().isEmpty())
        ui->issueButton->setEnabled(false);
    else if(modInfo_.issues().toString().contains("github.com"))
        ui->issueButton->setIcon(QIcon(":/image/github.svg"));

    QPixmap pixelmap;
    pixelmap.loadFromData(modInfo_.iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    ui->modIcon->setCursor(Qt::ArrowCursor);

    //file info
    auto fileInfo = modInfo_.fileInfo();
    auto [ baseName, suffix ] = modInfo_.baseNameFullSuffix();
    ui->fileBaseNameText->setText(baseName);
    ui->fileSuffixText->setText(suffix);
    ui->sizeText->setText(numberConvert(fileInfo.size(), "B"));
    ui->createdTimeText->setText(fileInfo.fileTime(QFile::FileBirthTime).toString());
    ui->modifiedTimeText->setText(fileInfo.fileTime(QFile::FileModificationTime).toString());

    if(mod_){
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

}

void LocalModInfoDialog::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod, modInfo_.fileInfo().path(), mod_);
    dialog->show();
}

void LocalModInfoDialog::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod, modInfo_.fileInfo().path(), mod_);
    dialog->show();
}


void LocalModInfoDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(modInfo_.website());
}


void LocalModInfoDialog::on_sourceButton_clicked()
{
    QDesktopServices::openUrl(modInfo_.sources());
}


void LocalModInfoDialog::on_issueButton_clicked()
{
    QDesktopServices::openUrl(modInfo_.issues());
}

void LocalModInfoDialog::on_editFileNameButton_clicked()
{
    ui->fileBaseNameText->setEnabled(true);
}


void LocalModInfoDialog::on_fileBaseNameText_editingFinished()
{
    auto [ baseName, suffix ] = modInfo_.baseNameFullSuffix();
    ui->fileBaseNameText->setEnabled(false);
    if(ui->fileBaseNameText->text() == baseName)
        return;
    qDebug() << ui->fileBaseNameText->text();
    if(!modInfo_.rename(ui->fileBaseNameText->text())) {
        ui->fileBaseNameText->setText(baseName);
        QMessageBox::information(this, tr("Rename Failed"), tr("Rename failed!"));
    }
}


void LocalModInfoDialog::on_oldModListWidget_doubleClicked(const QModelIndex &index)
{
    auto modInfo = mod_->oldInfos().at(index.row());
    auto dialog = new LocalModInfoDialog(this, modInfo);
    dialog->show();
}

