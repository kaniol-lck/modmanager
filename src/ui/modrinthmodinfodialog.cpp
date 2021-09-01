#include "modrinthmodinfodialog.h"
#include "ui_modrinthmodinfodialog.h"

#include <QTextDocument>
#include <QDebug>

#include "modrinthfileitemwidget.h"
#include "modrinth/modrinthmod.h"
#include "util/datetimesortitem.h"

ModrinthModInfoDialog::ModrinthModInfoDialog(QWidget *parent, ModrinthMod *mod) :
    QDialog(parent),
    ui(new Ui::ModrinthModInfoDialog),
    modrinthMod(mod)
{
    ui->setupUi(this);

    auto updateBasicInfo = [=]{
        setWindowTitle(mod->getModInfo().getName() + tr(" - Modrinth"));
        ui->modName->setText(mod->getModInfo().getName());
        ui->modSummary->setText(mod->getModInfo().getSummary());
            ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->getModInfo().getWebsiteUrl().toString()));
        ui->modAuthors->setText("by " + mod->getModInfo().getAuthor());

        //update icon
        //included by basic info
        auto updateIcon= [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod->getModInfo().getIconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!mod->getModInfo().getIconBytes().isEmpty())
            updateIcon();
        else {
            mod->acquireIcon();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(mod, &ModrinthMod::iconReady, this, updateIcon);
        }
    };

    auto bl = mod->getModInfo().hasBasicInfo();
    if(bl) updateBasicInfo();

    //update full info
    auto updateFullInfo = [=]{
        if(!bl) updateBasicInfo();
        auto text = mod->getModInfo().getDescription();
        text.replace("<br>", "\n");
        ui->modDescription->setMarkdown(text);
        ui->modDescription->setCursor(Qt::ArrowCursor);

        //update file list
        auto updateFileList = [=]{
            ui->fileListWidget->clear();
            auto files = mod->getModInfo().getFileList();
            for(const auto &fileInfo : qAsConst(files)){
                auto *listItem = new DateTimeSortItem();
                listItem->setData(DateTimeSortItem::Role, fileInfo.getFileDate());
                listItem->setSizeHint(QSize(500, 90));
                auto itemWidget = new ModrinthFileItemWidget(this, fileInfo);
                ui->fileListWidget->addItem(listItem);
                ui->fileListWidget->setItemWidget(listItem, itemWidget);
            }
            ui->fileListWidget->sortItems(Qt::DescendingOrder);
            ui->fileListWidget->setCursor(Qt::ArrowCursor);
        };

        if(!mod->getModInfo().getFileList().isEmpty())
            updateFileList();
        else {
            ui->fileListWidget->setCursor(Qt::BusyCursor);
            mod->acquireFileList();
            connect(mod, &ModrinthMod::fileListReady, this, updateFileList);
        }
    };

    if(!mod->getModInfo().getDescription().isEmpty())
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
