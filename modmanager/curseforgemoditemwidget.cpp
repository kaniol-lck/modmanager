#include "curseforgemoditemwidget.h"
#include "ui_curseforgemoditemwidget.h"

#include "curseforgemod.h"

CurseforgeModItemWidget::CurseforgeModItemWidget(QWidget *parent, CurseforgeMod *mod) :
    QWidget(parent),
    ui(new Ui::CurseforgeModItemWidget),
    curseforgeMod(mod)
{
    ui->setupUi(this);
    ui->modIcon->setFixedSize(80, 80);
    connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, &CurseforgeModItemWidget::updateThumbnail);

    ui->modName->setText(mod->getName());
    ui->modSummary->setText(mod->getSummary());
}

CurseforgeModItemWidget::~CurseforgeModItemWidget()
{
    delete ui;
}

void CurseforgeModItemWidget::updateThumbnail()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(curseforgeMod->getThumbnailBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}
