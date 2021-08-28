#include "modrinthmoditemwidget.h"
#include "ui_modrinthmoditemwidget.h"

ModrinthModItemWidget::ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod) :
    QWidget(parent),
    ui(new Ui::ModrinthModItemWidget),
    modrinthMod(mod)
{
    ui->setupUi(this);

    ui->modName->setText(mod->getModInfo().getName());
    ui->modSummary->setText(mod->getModInfo().getSummary());
    ui->modAuthors->setText("by <b>" + mod->getModInfo().getAuthor() + "</b>");

    connect(modrinthMod, &ModrinthMod::iconReady, this, &ModrinthModItemWidget::updateIcon);
}

ModrinthModItemWidget::~ModrinthModItemWidget()
{
    delete ui;
}

void ModrinthModItemWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(modrinthMod->getModInfo().getIconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}
