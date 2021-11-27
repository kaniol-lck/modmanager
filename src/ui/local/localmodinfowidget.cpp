#include "localmodinfowidget.h"
#include "ui_localmodinfowidget.h"

#include <QGraphicsDropShadowEffect>

#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/smoothscrollbar.h"
#include "util/flowlayout.h"
#include "util/funcutil.h"

LocalModInfoWidget::LocalModInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalModInfoWidget)
{
    ui->setupUi(this);
    ui->tagsWidget->setLayout(new FlowLayout());
    ui->scrollArea->setVisible(false);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
}

LocalModInfoWidget::~LocalModInfoWidget()
{
    delete ui;
}

void LocalModInfoWidget::setMod(LocalMod *mod)
{
    mod_ = mod;
    emit modChanged();

    ui->scrollArea->setVisible(mod_);
    if(!mod_) return;

    updateInfo();
    connect(this, &LocalModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &LocalMod::modFileUpdated, this, &LocalModInfoWidget::updateInfo)));

    auto onIconChanged = [=]{
        ui->modIcon->setPixmap(mod_->icon().scaled(80, 80));
    };
    onIconChanged();
    connect(this, &LocalModInfoWidget::modChanged, this, disconnecter(
                connect(mod_, &LocalMod::modIconUpdated, this, onIconChanged)));
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

    //tags
    for(auto widget : qAsConst(tagWidgets_)){
        ui->tagsWidget->layout()->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    for(auto &&tag : mod_->tagManager().tags()){
        auto label = new QLabel(tag.name(), this);
        label->setToolTip(tr("%1: %2").arg(tag.category().name(), tag.name()));
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.category().color().name()));
        ui->tagsWidget->layout()->addWidget(label);
        tagWidgets_ << label;
    }
}
