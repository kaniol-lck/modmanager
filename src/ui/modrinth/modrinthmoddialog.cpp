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

ModrinthModDialog::ModrinthModDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::ModrinthModDialog),
    mod_(mod),
    localMod_(localMod)
{
    ui->setupUi(this);
    auto action = new QAction(tr("Copy website link"), this);
    connect(action, &QAction::triggered, this, [=]{
        QApplication::clipboard()->setText(mod_->modInfo().websiteUrl().toString());
    });
    ui->websiteButton->addAction(action);

    auto updateBasicInfo = [=]{
        setWindowTitle(mod->modInfo().name() + tr(" - Modrinth"));
        ui->modName->setText(mod->modInfo().name());
        ui->modSummary->setText(mod->modInfo().summary());
        if(!mod->modInfo().author().isEmpty()){
            ui->modAuthors->setText(mod->modInfo().author());
            ui->modAuthors->setVisible(true);
            ui->author_label->setVisible(true);
        } else{
            ui->modAuthors->setVisible(false);
            ui->author_label->setVisible(false);
        }

        //update icon
        //included by basic info
        auto updateIcon= [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod->modInfo().iconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!mod->modInfo().iconBytes().isEmpty())
            updateIcon();
        else {
            mod->acquireIcon();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(mod, &ModrinthMod::iconReady, this, updateIcon);
        }
    };

    auto bl = mod->modInfo().hasBasicInfo();
    if(bl) updateBasicInfo();

    //update full info
    auto updateFullInfo = [=]{
        if(!bl) updateBasicInfo();
        auto text = mod->modInfo().description();
        QTextDocument doc;
        doc.setMarkdown(text);
        ui->modDescription->setHtml(doc.toHtml());
        ui->modDescription->setCursor(Qt::ArrowCursor);

        //update file list
        auto updateFileList = [=]{
            ui->fileListWidget->clear();
            auto files = mod->modInfo().fileList();
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
        };

        if(!mod->modInfo().fileList().isEmpty())
            updateFileList();
        else {
            ui->fileListWidget->setCursor(Qt::BusyCursor);
            mod->acquireFileList();
            connect(mod, &ModrinthMod::fileListReady, this, updateFileList);
        }
    };

    if(!mod->modInfo().description().isEmpty())
        updateFullInfo();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireFullInfo();
        connect(mod, &ModrinthMod::fullInfoReady, this, updateFullInfo);
    }
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

void ModrinthModDialog::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(mod_->modInfo().websiteUrl());
}
