#include "localmodfileitemwidget.h"
#include "ui_localmodfileitemwidget.h"

#include <QGraphicsDropShadowEffect>

#include "local/localmodfile.h"
#include "config.hpp"

LocalModFileItemWidget::LocalModFileItemWidget(QWidget *parent, LocalModFile *file) :
    QWidget(parent),
    ui(new Ui::LocalModFileItemWidget),
    file_(file)
{
    ui->setupUi(this);
    onFileChanged();
    connect(file, &LocalModFile::fileChanged, this, &LocalModFileItemWidget::onFileChanged);
}

LocalModFileItemWidget::~LocalModFileItemWidget()
{
    delete ui;
}

QRadioButton *LocalModFileItemWidget::button()
{
    return ui->radioButton;
}

void LocalModFileItemWidget::onFileChanged()
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
    QPixmap pixmap;
    pixmap.loadFromData(file_->commonInfo()->iconBytes());
    ui->modIcon->setPixmap(pixmap.scaled(80, 80));
    auto name = file_->commonInfo()->name();
    setEffect(ui->modName, name);
    ui->modName->setText(name);
    ui->modVersion->setText(file_->commonInfo()->version());
    ui->modAuthors->setVisible(Config().getShowModAuthors());
    //authors
    if (!file_->commonInfo()->authors().isEmpty()){
        auto authors = file_->commonInfo()->authors().join("</b>, <b>").prepend("by <b>").append("</b>");
        setEffect(ui->modAuthors, authors);
        ui->modAuthors->setText(authors);
    }else
        ui->modAuthors->setText("");
    auto description = file_->commonInfo()->description();
    ui->modDescription->setText(description);
    setEffect(ui->modDescription, description);
}
