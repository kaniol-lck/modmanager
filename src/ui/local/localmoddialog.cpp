#include "localmoddialog.h"
#include "ui_localmoddialog.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "ui/modrinth/modrinthmoddialog.h"
#include "ui/local/localmodmenu.h"
#include "util/funcutil.h"
#include "util/websiteicon.h"
#include "util/flowlayout.h"

LocalModDialog::LocalModDialog(QWidget *parent, LocalMod *mod) :
    QDialog(parent),
    ui(new Ui::LocalModDialog),
    file_(mod->modFile()),
    mod_(mod)
{
    ui->setupUi(this);
    //not use it currently
    ui->disableButton->setVisible(false);
    ui->aliasText->setVisible(false);

    tagsLayout_ = new FlowLayout();
    ui->tagsHorizontal->addLayout(tagsLayout_);

    connect(mod_, &LocalMod::destroyed, this, &QDialog::close);

    //init info
    onCurrentModChanged();

    //signals / slots
    connect(mod_, &LocalMod::curseforgeReady, this, [=](bool bl){
        ui->curseforgeButton->setEnabled(bl);
    });
    connect(mod_, &LocalMod::modrinthReady, this, [=](bool bl){
        ui->modrinthButton->setEnabled(bl);
    });
    connect(mod_, &LocalMod::modFileUpdated, this, &LocalModDialog::onCurrentModChanged);
    connect(ui->aliasText, &QLineEdit::returnPressed, this, [=]{
        ui->editAliasButton->setChecked(false);
    });
    connect(ui->fileBaseNameText, &QLineEdit::returnPressed, this, [=]{
        ui->editFileNameButton->setChecked(false);
    });
}

LocalModDialog::~LocalModDialog()
{
    delete ui;
}

void LocalModDialog::onCurrentModChanged()
{
    file_ = mod_->modFile();
    if(mod_->curseforgeMod()) ui->curseforgeButton->setEnabled(true);
    if(mod_->modrinthMod()) ui->modrinthButton->setEnabled(true);
    ui->modName->setText(mod_->alias());
    ui->aliasText->setText(mod_->alias());

    ui->versionSelect->clear();
    ui->versionSelect->addItem(mod_->modFile()->commonInfo()->version());
    ui->versionText->setText(mod_->modFile()->commonInfo()->version());
    for(auto file : mod_->oldFiles())
        ui->versionSelect->addItem(file->commonInfo()->version());
    ui->versionSelect->setCurrentText(file_->commonInfo()->version());
    ui->versionSelect->setVisible(!mod_->oldFiles().empty());
    ui->versionText->setVisible(mod_->oldFiles().empty());

    onCurrentFileChanged();
}

void LocalModDialog::onCurrentFileChanged()
{
    auto setEffect = [=](QWidget *widget, const QString str){
        if(str.contains("</span>")){
            auto *effect = new QGraphicsDropShadowEffect;
            effect->setBlurRadius(4);
            effect->setColor(Qt::darkGray);
            effect->setOffset(1, 1);
            widget->setGraphicsEffect(effect);
        } else
            widget->setGraphicsEffect(nullptr);
    };

    ui->rollbackButton->setVisible(file_ != mod_->modFile());
    ui->deleteButton->setVisible(file_ != mod_->modFile());
    ui->aliasText->setPlaceholderText(file_->commonInfo()->name());

    auto str = ui->modName->text();
    if(file_ == mod_->modFile())
        str.append(" - " + tr("Local"));
    else
        str.append(" - " + tr("Local Old"));
    setWindowTitle(clearFormat(str));

    //mod name
    auto displayName = mod_->displayName();
    if(mod_->isDisabled())
        displayName.append(" (Disabled)");
    setEffect(ui->modName, displayName);
    ui->modName->setText(displayName);
    //authors
    if (!mod_->commonInfo()->authors().isEmpty()){
        auto authors = mod_->commonInfo()->authors().join("</b>, <b>").prepend("by <b>").append("</b>");
        setEffect(ui->modAuthors, authors);
        ui->modAuthors->setText(authors);
    }
    else
        ui->modAuthors->setText("");
    //description
    auto description = mod_->commonInfo()->description();
    setEffect(ui->modDescription, description);
    ui->modDescription->setText(description);

    ui->homepageButton->setEnabled(false);
    ui->sourceButton->setEnabled(false);
    ui->issueButton->setEnabled(false);

    if(!file_->commonInfo()->homepage().isEmpty()){
        ui->homepageButton->setEnabled(true);
        auto homepageIcon = new WebsiteIcon(this);
        connect(homepageIcon, &WebsiteIcon::iconGot, this, [=](const auto &icon){
            ui->homepageButton->setIcon(icon);
        });
        homepageIcon->get(file_->commonInfo()->homepage());
    }

    if(!file_->commonInfo()->sources().isEmpty()){
        ui->sourceButton->setEnabled(true);
        auto homepageIcon = new WebsiteIcon(this);
        connect(homepageIcon, &WebsiteIcon::iconGot, this, [=](const auto &icon){
            ui->sourceButton->setIcon(icon);
        });
        homepageIcon->get(file_->commonInfo()->sources());
    }

    if(!file_->commonInfo()->issues().isEmpty()){
        ui->issueButton->setEnabled(true);
        auto homepageIcon = new WebsiteIcon(this);
        connect(homepageIcon, &WebsiteIcon::iconGot, this, [=](const auto &icon){
            ui->issueButton->setIcon(icon);
        });
        homepageIcon->get(file_->commonInfo()->issues());
    }

    auto setIcon = [=](auto &&image) {
        QPixmap pixelmap;
        if (mod_->modFile()->type() == LocalModFile::Disabled){
            auto alphaChannel = image.convertToFormat(QImage::Format_Alpha8);
            image = image.convertToFormat(QImage::Format_Grayscale8);
            image.setAlphaChannel(alphaChannel);
        }
        pixelmap.convertFromImage(image);
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    };

    if(!mod_->commonInfo()->iconBytes().isEmpty()){
        QImage image;
        image.loadFromData(mod_->commonInfo()->iconBytes());
        setIcon(std::move(image));
    }else if(mod_->commonInfo()->id() == "optifine"){
        setIcon(QImage(":/image/optifine.png"));
    }else if(mod_->curseforgeMod()){
        auto setCurseforgeIcon = [=]{
            QImage image;
            image.loadFromData(mod_->curseforgeMod()->modInfo().iconBytes());
            setIcon(std::move(image));
        };
        if(!mod_->curseforgeMod()->modInfo().iconBytes().isEmpty())
            setCurseforgeIcon();
        else{
            mod_->curseforgeMod()->acquireBasicInfo();
            connect(mod_->curseforgeMod(), &CurseforgeMod::basicInfoReady, this, [=]{
                mod_->curseforgeMod()->acquireIcon();
                connect(mod_->curseforgeMod(), &CurseforgeMod::iconReady, this, setCurseforgeIcon);
            });
        }
    } else if(mod_->modrinthMod()){
        auto setModrinthIcon = [=]{
            QImage image;
            image.loadFromData(mod_->modrinthMod()->modInfo().iconBytes());
            setIcon(image);
        };
        if(!mod_->modrinthMod()->modInfo().iconBytes().isEmpty())
            setModrinthIcon();
        else{
            mod_->modrinthMod()->acquireFullInfo();
            connect(mod_->modrinthMod(), &ModrinthMod::fullInfoReady, this, [=]{
                connect(mod_->modrinthMod(), &ModrinthMod::iconReady, this, setModrinthIcon);
                mod_->modrinthMod()->acquireIcon();
            });
        }
    } else
        setIcon(QImage(":/image/modmanager.png"));

    //file info
    auto fileInfo = file_->fileInfo();
    auto [ baseName, suffix ] = file_->baseNameFullSuffix();
    ui->fileBaseNameText->setText(baseName);
    ui->fileSuffixText->setText(suffix);
    ui->sizeText->setText(sizeConvert(fileInfo.size()));
    ui->createdTimeText->setText(fileInfo.fileTime(QFile::FileBirthTime).toString());
    ui->modifiedTimeText->setText(fileInfo.fileTime(QFile::FileModificationTime).toString());

    //if disabled
    ui->disableButton->setChecked(mod_->isDisabled());

    //tags
//    ui->tagsWidget->setVisible(!mod_->tags().isEmpty());
    QStringList tagTextList;
    for(auto widget : qAsConst(tagWidgets_)){
        tagsLayout_->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    for(auto &&tag : mod_->tags()){
        auto label = new QLabel(tag.name(), this);
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        tagsLayout_->addWidget(label);
        tagWidgets_ << label;
    }
}

void LocalModDialog::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModDialog(this, curseforgeMod, mod_);
    dialog->show();
}

void LocalModDialog::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModDialog(this, modrinthMod, mod_);
    dialog->show();
}

void LocalModDialog::on_homepageButton_clicked()
{
    QDesktopServices::openUrl(file_->commonInfo()->homepage());
}

void LocalModDialog::on_sourceButton_clicked()
{
    QDesktopServices::openUrl(file_->commonInfo()->sources());
}

void LocalModDialog::on_issueButton_clicked()
{
    QDesktopServices::openUrl(file_->commonInfo()->issues());
}

void LocalModDialog::on_disableButton_toggled(bool checked)
{
    ui->disableButton->setEnabled(false);
    mod_->setEnabled(!checked);
    ui->disableButton->setEnabled(true);
}

void LocalModDialog::on_editAliasButton_toggled(bool checked)
{
    ui->aliasText->setVisible(checked);
    ui->modName->setVisible(!checked);
    if(checked)
        //start edit
        ui->aliasText->setFocus();
    else{
        //finish edit
        mod_->setAlias(ui->aliasText->text());
    }
}

void LocalModDialog::on_editFileNameButton_toggled(bool checked)
{
    ui->fileBaseNameText->setEnabled(checked);
    if(checked){
        //start edit
        ui->fileBaseNameText->setFocus();
    } else{
        QString baseName;
        if(isRenaming) return;
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
}

void LocalModDialog::on_versionSelect_currentIndexChanged(int index)
{
    if(index < 0 || index > mod_->oldFiles().size()) return;
    if(index == 0)
        file_ = mod_->modFile();
    else
        file_ = mod_->oldFiles().at(index - 1);
    onCurrentFileChanged();
}

void LocalModDialog::on_rollbackButton_clicked()
{
    mod_->rollback(file_);
}

void LocalModDialog::on_deleteButton_clicked()
{
    if(QMessageBox::No == QMessageBox::question(this, tr("Delete"), tr("Delete this version?"))) return;
    mod_->deleteOld(file_);
}

void LocalModDialog::on_tagsWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    auto localModMenu = new LocalModMenu(this, mod_);
    menu->addMenu(localModMenu->addTagMenu());
    if(!mod_->tags().isEmpty())
        menu->addMenu(localModMenu->removeTagmenu());
    menu->exec(ui->tagsWidget->mapToGlobal(pos));
}
