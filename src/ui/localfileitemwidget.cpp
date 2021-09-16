#include "localfileitemwidget.h"
#include "ui_localfileitemwidget.h"

#include "local/localmod.h"
#include "local/localmodfile.h"
#include "util/funcutil.h"

LocalFileItemWidget::LocalFileItemWidget(QWidget *parent, LocalMod *mod, LocalModFile *file) :
    QWidget(parent),
    ui(new Ui::LocalFileItemWidget),
    mod_(mod),
    file_(file)
{
    ui->setupUi(this);

    updateFileInfo();
    connect(file_, &LocalModFile::fileChanged, this, &LocalFileItemWidget::updateFileInfo);
}

LocalFileItemWidget::~LocalFileItemWidget()
{
    delete ui;
}

void LocalFileItemWidget::updateFileInfo()
{
    auto info = file_->commonInfo();
    QPixmap pixmap;
    pixmap.loadFromData(info->iconBytes());
    ui->modIcon->setPixmap(pixmap.scaled(80, 80));
    ui->displayNameText->setText(info->name());
    ui->versionText->setText(info->version());
    ui->modFileNameText->setText(file_->fileInfo().fileName());
    ui->sizeText->setText(numberConvert(file_->fileInfo().size(), "B"));
}

void LocalFileItemWidget::on_rollbackButton_clicked()
{
    mod_->rollback(file_);
}

void LocalFileItemWidget::on_openFolderButton_clicked()
{
    openFileInFolder(file_->path());
}

