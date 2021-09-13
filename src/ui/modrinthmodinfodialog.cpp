#include "modrinthmodinfodialog.h"
#include "ui_modrinthmodinfodialog.h"

#include <QTextDocument>
#include <QDebug>

#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "util/datetimesortitem.h"

ModrinthModInfoDialog::ModrinthModInfoDialog(QWidget *parent, ModrinthMod *mod, const QString &path, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::ModrinthModInfoDialog),
    mod_(mod),
    localMod_(localMod),
    downloadPath_(path)
{
    ui->setupUi(this);

    auto updateBasicInfo = [=]{
        setWindowTitle(mod->modInfo().name() + tr(" - Modrinth"));
        ui->modName->setText(mod->modInfo().name());
        ui->modSummary->setText(mod->modInfo().summary());
            ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->modInfo().websiteUrl().toString()));
        ui->modAuthors->setText("by " + mod->modInfo().author());

        //update icon
        //included by basic info
        auto updateIcon= [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod->modInfo().iconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
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
        text.replace("<br>", "\n");
        ui->modDescription->setMarkdown(text);
        ui->modDescription->setCursor(Qt::ArrowCursor);

        //update file list
        auto updateFileList = [=]{
            ui->fileListWidget->clear();
            auto files = mod->modInfo().fileList();
            for(const auto &fileInfo : qAsConst(files)){
                auto *listItem = new DateTimeSortItem();
                listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
                listItem->setSizeHint(QSize(500, 90));
                auto itemWidget = new ModrinthFileItemWidget(this, mod_, fileInfo, downloadPath_, localMod_);
                connect(this, &ModrinthModInfoDialog::downloadPathChanged, itemWidget, &ModrinthFileItemWidget::setDownloadPath);
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

}

ModrinthModInfoDialog::~ModrinthModInfoDialog()
{
    delete ui;
}

void ModrinthModInfoDialog::setDownloadPath(const QString &newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}
