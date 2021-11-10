#include "modrinthmoddialog.h"
#include "ui_modrinthmoddialog.h"

#include <QDesktopServices>
#include <QTextDocument>
#include <QDebug>
#include <QAction>
#include <QClipboard>

#include "local/localmodpath.h"
#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "util/datetimesortitem.h"
#include "util/smoothscrollbar.h"

ModrinthModDialog::ModrinthModDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::ModrinthModDialog),
    mod_(mod),
    localMod_(localMod)
{
    ui->setupUi(this);
    ui->modDescription->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->fileListWidget->setVerticalScrollBar(new SmoothScrollBar(this));

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
    connect(mod_, &ModrinthMod::fileListReady, this, &ModrinthModDialog::updateFileList);
    connect(mod_, &ModrinthMod::iconReady, this, &ModrinthModDialog::updateIcon);

    if(localMod_) setDownloadPath(localMod_->path());
}

ModrinthModDialog::~ModrinthModDialog()
{
    delete ui;
}

void ModrinthModDialog::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}

void ModrinthModDialog::updateBasicInfo()
{
    auto action = new QAction(tr("Copy website link"), this);
    connect(action, &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
    });
    ui->websiteButton->addAction(action);

    setWindowTitle(mod_->modInfo().name() + tr(" - Modrinth"));
    ui->modName->setText(mod_->modInfo().name());
    ui->modSummary->setText(mod_->modInfo().summary());
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
    text.replace(QRegExp("<br ?/?>"), "\n");
    ui->websiteButton->setVisible(!mod_->modInfo().websiteUrl().isEmpty());
    ui->modDescription->setMarkdown(text);
    ui->modDescription->setCursor(Qt::ArrowCursor);

    //update file list
    if(!mod_->modInfo().fileList().isEmpty())
        updateFileList();
    else {
        ui->fileListWidget->setCursor(Qt::BusyCursor);
        mod_->acquireFileList();
    }
}

void ModrinthModDialog::updateFileList()
{
    ui->fileListWidget->clear();
    auto files = mod_->modInfo().fileList();
    for(const auto &fileInfo : qAsConst(files)){
        auto *listItem = new DateTimeSortItem();
        listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
        listItem->setSizeHint(QSize(500, 90));
        auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo, localMod_);
        itemWidget->setDownloadPath(downloadPath_);
        connect(this, &ModrinthModDialog::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
        ui->fileListWidget->addItem(listItem);
        ui->fileListWidget->setItemWidget(listItem, itemWidget);
    }
    ui->fileListWidget->sortItems(Qt::DescendingOrder);
    ui->fileListWidget->setCursor(Qt::ArrowCursor);
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
