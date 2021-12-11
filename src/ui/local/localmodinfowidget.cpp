#include "localmodinfowidget.h"
#include "ui_localmodinfowidget.h"

#include <QGraphicsDropShadowEffect>

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

LocalModInfoWidget::LocalModInfoWidget(QWidget *parent, LocalModPath *path) :
    DockWidgetContent(parent),
    path_(path),
    ui(new Ui::LocalModInfoWidget)
{
    ui->setupUi(this);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
    if(path_)
        ui->pathName->setText(path_->info().displayName());
}

LocalModInfoWidget::~LocalModInfoWidget()
{
    delete ui;
}

void LocalModInfoWidget::setMods(QList<LocalMod *> mods)
{
    if(mods == mods_) return;
    mods_ = mods;
    emit modChanged();

    switch (mods.size()) {
    case 0:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 1:{
        ui->stackedWidget->setCurrentIndex(1);
        mod_ = mods.first();
        ui->tagsWidget->setMod(mod_);

        updateInfo();
        connect(this, &LocalModInfoWidget::modChanged, this, disconnecter(
                    connect(mod_, &LocalMod::modInfoChanged, this, &LocalModInfoWidget::updateInfo)));

        auto onIconChanged = [=]{
            ui->modIcon->setPixmap(mod_->icon().scaled(80, 80, Qt::KeepAspectRatio));
        };
        onIconChanged();
        connect(this, &LocalModInfoWidget::modChanged, this, disconnecter(
                    connect(mod_, &LocalMod::modIconUpdated, this, onIconChanged)));
        break;
    }
    default:
        ui->stackedWidget->setCurrentIndex(0);
        ui->modCount->setText(tr("%1 mods are selected.").arg(mods.size()));
    }
}

void LocalModInfoWidget::updateInfo()
{
    ui->scrollArea->setVisible(mod_);
    if(!mod_) return;
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

    //mod name
    auto displayName = mod_->displayName();
    setEffect(ui->modName, displayName);
    ui->modName->setText(displayName);
//    ui->modVersion->setText(mod_->commonInfo()->version());
    auto description = mod_->commonInfo()->description();
    //description
    setEffect(ui->modDescription, description);
    ui->modDescription->setText(description);
//        //authors
//        if (!mod_->commonInfo()->authors().isEmpty()){
//            auto authors = mod_->commonInfo()->authors().join("</b>, <b>").prepend("by <b>").append("</b>");
//            setEffect(ui->modAuthors, authors);
//            ui->modAuthors->setText(authors);
//        }
//        else
//            ui->modAuthors->setText("");

//    if (mod_->curseforgeMod())
//        ui->curseforgeButton->setVisible(true);
//    if (mod_->modrinthMod())
//        ui->modrinthButton->setVisible(true);
}
