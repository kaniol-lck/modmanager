#include "modrinthmodinfowidget.h"
#include "ui_modrinthmodinfowidget.h"

#include <QMenu>

#include "modrinthmodbrowser.h"
#include "modrinthmoddialog.h"
#include "modrinth/modrinthmod.h"
#include "local/localmodpath.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"
#include "util/youdaotranslator.h"

ModrinthModInfoWidget::ModrinthModInfoWidget(ModrinthModBrowser *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthModInfoWidget),
    browser_(parent)
{
    ui->setupUi(this);
    ui->scrollArea->setVisible(false);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
}

ModrinthModInfoWidget::~ModrinthModInfoWidget()
{
    delete ui;
}

void ModrinthModInfoWidget::setMod(ModrinthMod *mod)
{
    mod_ = mod;
    emit modChanged();
    connect(this, &ModrinthModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &ModrinthMod::fullInfoReady, this, &ModrinthModInfoWidget::updateFullInfo),
                connect(mod_, &ModrinthMod::iconReady, this, &ModrinthModInfoWidget::updateIcon),
                connect(mod_, &QObject::destroyed, this, [=]{ setMod(nullptr); })));


    ui->scrollArea->setVisible(mod_);
    ui->tagsWidget->setTagableObject(mod_);
    if(!mod_) return;

//    auto action = new QAction(QIcon::fromTheme("edit-copy"), tr("Copy website link"), this);
//    connect(action, &QAction::triggered, this, [=]{
//        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
//    });
//    ui->websiteButton->addAction(action);

    updateBasicInfo();

    //update full info
    updateFullInfo();
    if(mod_->modInfo().description().isEmpty()){
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod_->acquireFullInfo();
    }
}

void ModrinthModInfoWidget::updateBasicInfo()
{
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
//            ui->modAuthors->setText(mod->modInfo().author());
//            ui->modAuthors->setVisible(true);
//            ui->author_label->setVisible(true);
    } else{
//            ui->modAuthors->setVisible(false);
//            ui->author_label->setVisible(false);
    }

    //update icon
    //included by basic info
    updateIcon();
    if(mod_->modInfo().icon().isNull()){
        mod_->acquireIcon();
        ui->modIcon->setCursor(Qt::BusyCursor);
    }
}

void ModrinthModInfoWidget::updateFullInfo()
{
    updateBasicInfo();
    auto text = mod_->modInfo().description();
    text.replace(QRegularExpression("<br ?/?>"), "\n");
//        ui->websiteButton->setVisible(!mod_->modInfo().websiteUrl().isEmpty());
    ui->modDescription->setMarkdown(text);
    ui->modDescription->setCursor(Qt::ArrowCursor);
}

void ModrinthModInfoWidget::updateIcon()
{
    ui->modIcon->setPixmap(mod_->modInfo().icon().scaled(80, 80, Qt::KeepAspectRatio));
    ui->modIcon->setCursor(Qt::ArrowCursor);
}

void ModrinthModInfoWidget::on_modSummary_customContextMenuRequested(const QPoint &pos)
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

void ModrinthModInfoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(mod_ && !mod_->parent()){
        auto dialog = new ModrinthModDialog(browser_, mod_);
        //set parent
        mod_->setParent(dialog);
        connect(dialog, &ModrinthModDialog::finished, this, [=]{
            mod_->setParent(nullptr);
        });
        dialog->show();
    }
    QWidget::mouseDoubleClickEvent(event);
}

