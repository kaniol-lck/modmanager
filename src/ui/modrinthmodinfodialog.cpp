#include "modrinthmodinfodialog.h"
#include "ui_modrinthmodinfodialog.h"

#include <modrinth/modrinthmod.h>

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
        //    ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->getModInfo().getWebsiteUrl().toString()));
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

    updateBasicInfo();

    //update full info
    auto updateFullInfo = [=]{
        updateBasicInfo();
        ui->modDescription->setText(mod->getModInfo().getDescription());
        ui->modDescription->setCursor(Qt::ArrowCursor);
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
