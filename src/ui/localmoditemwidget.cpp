#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include <QMenu>
#include <QMessageBox>

#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "curseforgemodinfodialog.h"
#include "modrinthmodinfodialog.h"
#include "util/tutil.hpp"

LocalModItemWidget::LocalModItemWidget(QWidget *parent, LocalMod *mod) :
    QWidget(parent),
    ui(new Ui::LocalModItemWidget),
    mod_(mod)
{
    //init ui
    ui->setupUi(this);
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->curseforgeButton->setEnabled(false);
    ui->modrinthButton->setEnabled(false);

    //init info
    updateInfo();

    //signals / slots
    connect(mod_, &LocalMod::modFileUpdated, this, &LocalModItemWidget::updateInfo);

    connect(mod_, &LocalMod::updateReady, this, &LocalModItemWidget::updateReady);

    connect(mod_, &LocalMod::checkCurseforgeStarted, this, &LocalModItemWidget::startCheckCurseforge);
    connect(mod_, &LocalMod::curseforgeReady, this, &LocalModItemWidget::curseforgeReady);
    connect(mod_, &LocalMod::checkCurseforgeUpdateStarted, this, &LocalModItemWidget::startCheckCurseforgeUpdate);

    connect(mod_, &LocalMod::checkModrinthStarted, this, &LocalModItemWidget::startCheckModrinth);
    connect(mod_, &LocalMod::modrinthReady, this, &LocalModItemWidget::modrinthReady);
    connect(mod_, &LocalMod::checkModrinthUpdateStarted, this, &LocalModItemWidget::startCheckModrinthUpdate);

    connect(mod_, &LocalMod::updateStarted, this, &LocalModItemWidget::startUpdate);
    connect(mod_, &LocalMod::updateProgress, this, &LocalModItemWidget::updateProgress);
    connect(mod_, &LocalMod::updateFinished, this, &LocalModItemWidget::finishUpdate);
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

void LocalModItemWidget::updateInfo()
{
    ui->modName->setText(mod_->modFile()->commonInfo()->name());
    ui->modVersion->setText(mod_->commonInfo()->version());
    auto description = mod_->commonInfo()->description();
    auto index = description.indexOf(".");
    if(index > 0)
        description = description.left(index + 1);
    ui->modDescription->setText(description);
    if(!mod_->commonInfo()->authors().isEmpty())
        ui->modAuthors->setText(mod_->commonInfo()->authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    else
        ui->modAuthors->setText("");

    if(!mod_->commonInfo()->iconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(mod_->commonInfo()->iconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }

    //rollback
    if(mod_->oldFiles().isEmpty())
        ui->rollbackButton->setVisible(false);
    else{
        ui->rollbackButton->setVisible(true);
        ui->rollbackButton->setEnabled(true);
        auto menu = new QMenu(this);
        for(const auto &file : mod_->oldFiles())
            connect(menu->addAction(file->commonInfo()->version()), &QAction::triggered, this, [=]{
                ui->rollbackButton->setEnabled(false);
                mod_->rollback(file);
            });
        ui->rollbackButton->setMenu(menu);
    }

    //warning
    if(!mod_->duplicateFiles().isEmpty()){
        ui->warningButton->setToolTip(tr("Duplicate mod!"));
    } else {
        ui->warningButton->setVisible(false);
    }
}

void LocalModItemWidget::on_updateButton_clicked()
{
    mod_->update(mod_->defaultUpdateType());
}

void LocalModItemWidget::updateReady(LocalMod::ModWebsiteType type)
{
    if(type == LocalMod::None) return;
    ui->updateButton->setVisible(true);
    if(type == LocalMod::Curseforge)
        ui->updateButton->setIcon(QIcon(":/image/curseforge.svg"));
    else if (type == LocalMod::Modrinth)
        ui->updateButton->setIcon(QIcon(":/image/modrinth.svg"));

    if(mod_->updateTypes().size() == 1) return;
    auto menu = new QMenu(this);
    for(auto type2 : mod_->updateTypes()){
        if(type == type2) continue;
        if(type2 == LocalMod::Curseforge)
            connect(menu->addAction(QIcon(":/image/curseforge.svg"), "Curseforge"), &QAction::triggered, this, [=]{
                ui->updateButton->setIcon(QIcon(":/image/curseforge.svg"));
                mod_->update(LocalMod::Curseforge);
            });
        else if(type2 == LocalMod::Modrinth)
            connect(menu->addAction(QIcon(":/image/modrinth.svg"), "Modrinth"), &QAction::triggered, this, [=]{
                ui->updateButton->setIcon(QIcon(":/image/modrinth.svg"));
                mod_->update(LocalMod::Modrinth);
            });
    }
    ui->updateButton->setMenu(menu);
}

void LocalModItemWidget::startCheckCurseforge()
{
    //TODO
}

void LocalModItemWidget::curseforgeReady(bool bl)
{
    ui->curseforgeButton->setEnabled(bl);
}

void LocalModItemWidget::startCheckCurseforgeUpdate()
{
    //TODO
}

void LocalModItemWidget::startCheckModrinth()
{
    //TODO
}

void LocalModItemWidget::modrinthReady(bool bl)
{
    ui->modrinthButton->setEnabled(bl);
}

void LocalModItemWidget::startCheckModrinthUpdate()
{
    //TODO
}

void LocalModItemWidget::startUpdate()
{
    ui->updateButton->setText(tr("Updating"));
    ui->updateButton->setEnabled(false);
    ui->updateProgress->setVisible(true);
}

void LocalModItemWidget::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->updateProgress->setMaximum(bytesTotal);
    ui->updateProgress->setValue(bytesReceived);
}

void LocalModItemWidget::finishUpdate(bool success)
{
    ui->updateProgress->setVisible(false);
    if(success)
        ui->updateButton->setVisible(false);
    else{
        ui->updateButton->setText(tr("Retry Update"));
        ui->updateButton->setEnabled(true);
    }
}

void LocalModItemWidget::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if(!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod, mod_->modFile()->fileInfo().absolutePath(), mod_);
    dialog->show();
}

void LocalModItemWidget::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if(!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModInfoDialog(this, modrinthMod, mod_->modFile()->fileInfo().absolutePath(), mod_);
    dialog->show();
}

void LocalModItemWidget::on_warningButton_clicked()
{
    QString str = tr("Duplicate version of <b>%1</b> was found:").arg(mod_->commonInfo()->name());
    QStringList list(mod_->commonInfo()->version());
    for(const auto &file : mod_->duplicateFiles())
        list << file->commonInfo()->version();
    str += "<ul><li>" + list.join("</li><li>") + "</li></ul>";
    str += tr("Keep one of them and set the others as old mods?");
    if(QMessageBox::Yes == QMessageBox::question(this, tr("Incompatibility"), str))
        mod_->duplicateToOld();
}

