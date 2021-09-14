#include "localfileitemwidget.h"
#include "ui_localfileitemwidget.h"

#include "local/localmod.h"
#include "util/funcutil.h"

LocalFileItemWidget::LocalFileItemWidget(QWidget *parent, LocalMod *mod, const LocalModInfo &info) :
    QWidget(parent),
    ui(new Ui::LocalFileItemWidget),
    mod_(mod),
    info_(info)
{
    ui->setupUi(this);
    QPixmap pixmap;
    pixmap.loadFromData(info_.iconBytes());
    ui->modIcon->setPixmap(pixmap.scaled(80, 80));
    ui->displayNameText->setText(info_.name());
    ui->versionText->setText(info_.version());
    ui->modFileNameText->setText(info_.fileInfo().fileName());
    ui->sizeText->setText(numberConvert(info_.fileInfo().size(), "B"));
}

LocalFileItemWidget::~LocalFileItemWidget()
{
    delete ui;
}

void LocalFileItemWidget::on_rollbackButton_clicked()
{
    mod_->rollback(info_);
}

void LocalFileItemWidget::on_openFolderButton_clicked()
{
    openFileInFolder(info_.path());
}

