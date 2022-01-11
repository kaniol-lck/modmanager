#include "modrinthmodbrowser.h"
#include "modrinthmoddialog.h"
#include "ui_modrinthmoddialog.h"

#include <QDesktopServices>
#include <QTextDocument>
#include <QDebug>
#include <QAction>
#include <QClipboard>
#include <QMenu>

#include "local/localmodpath.h"
#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"
#include "util/youdaotranslator.h"

ModrinthModDialog::ModrinthModDialog(ModrinthModBrowser *parent, ModrinthMod *mod) :
    ModrinthModDialog(static_cast<QWidget *>(parent), mod)
{
    ui->fileList->setBrowser(parent);
}

ModrinthModDialog::ModrinthModDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::ModrinthModDialog),
    mod_(mod),
    localMod_(localMod)
{
    ui->setupUi(this);
    ui->fileList->setMod(mod_);
    ui->fileList->setLocalMod(localMod);
    ui->tagsWidget->setTagableObject(mod_);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));

    connect(mod_, &ModrinthMod::destroyed, this, &QDialog::close);

    auto bl = mod->modInfo().hasBasicInfo();
    if(bl) updateBasicInfo();

    //update full info
    if(!mod->modInfo().description().isEmpty())
        updateFullInfo();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireFullInfo();
    }
    connect(mod_, &ModrinthMod::fullInfoReady, this, &ModrinthModDialog::updateFullInfo);
    connect(mod_, &ModrinthMod::iconReady, this, &ModrinthModDialog::updateIcon);
}

ModrinthModDialog::~ModrinthModDialog()
{
    delete ui;
}

void ModrinthModDialog::updateBasicInfo()
{
    auto action = new QAction(QIcon::fromTheme("edit-copy"), tr("Copy website link"), this);
    connect(action, &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
    });
    ui->websiteButton->addAction(action);

    setWindowTitle(mod_->modInfo().name() + tr(" - Modrinth"));
    ui->modName->setText(mod_->modInfo().name());
    ui->modSummary->setText(mod_->modInfo().summary());
    if(Config().getAutoTranslate()){
        YoudaoTranslator::translator()->translate(mod_->modInfo().summary(), [=](const auto &translted){
            if(!translted.isEmpty())
                ui->modSummary->setText(translted);
            transltedSummary_ = true;
        });
    }
    if(!mod_->modInfo().author().isEmpty()){
        ui->modAuthors->setText(mod_->modInfo().author());
        ui->modAuthors->setVisible(true);
        ui->author_label->setVisible(true);
    } else{
        ui->modAuthors->setVisible(false);
        ui->author_label->setVisible(false);
    }

    //update icon
    //included by basic info
    if(!mod_->modInfo().iconBytes().isEmpty())
        updateIcon();
    else {
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }
}

void ModrinthModDialog::updateFullInfo()
{
    updateBasicInfo();
    auto text = mod_->modInfo().description();
    text.replace(QRegularExpression("<br ?/?>"), "\n");
    ui->websiteButton->setVisible(!mod_->modInfo().websiteUrl().isEmpty());
    ui->modDescription->setMarkdown(text);
    ui->modDescription->setCursor(Qt::ArrowCursor);
}

void ModrinthModDialog::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}

void ModrinthModDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().websiteUrl());
}

void ModrinthModDialog::on_modSummary_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    if(!transltedSummary_)
        menu->addAction(tr("Translate summary"), this, [=]{
            YoudaoTranslator::translator()->translate(mod_->modInfo().summary(), [=](const QString &translated){
                if(!translated.isEmpty()){
                    ui->modSummary->setText(translated);
                transltedSummary_ = true;
                }
            });
        });
    else{
        transltedSummary_ = false;
        menu->addAction(tr("Untranslate summary"), this, [=]{
            ui->modSummary->setText(mod_->modInfo().summary());
        });
    }
    menu->exec(ui->modSummary->mapToGlobal(pos));
}

