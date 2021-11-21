#include "localmodinfowidget.h"
#include "ui_localmodinfowidget.h"

#include <QGraphicsDropShadowEffect>

#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/smoothscrollbar.h"
#include "util/flowlayout.h"

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
    updateInfo();
    connect(mod_, &LocalMod::modFileUpdated, this, &LocalModInfoWidget::updateInfo);
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
    auto index = description.indexOf(".");
    //description
    //show first statements only
    if (index > 0)
        description = description.left(index + 1);
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
            connect(mod_->curseforgeMod(), &CurseforgeMod::basicInfoReady, this, [=]{
                connect(mod_->curseforgeMod(), &CurseforgeMod::iconReady, this, setCurseforgeIcon);
                mod_->curseforgeMod()->acquireIcon();
            });
            mod_->curseforgeMod()->acquireBasicInfo();
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
