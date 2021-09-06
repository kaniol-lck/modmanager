#include "modrinthmoditemwidget.h"
#include "ui_modrinthmoditemwidget.h"

#include <QMenu>

#include "util/funcutil.h"

ModrinthModItemWidget::ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod) :
    QWidget(parent),
    ui(new Ui::ModrinthModItemWidget),
    mod_(mod)
{
    ui->setupUi(this);
    ui->downloadProgress->setVisible(false);

    //TODO
    ui->downloadButton->setVisible(false);

    ui->modName->setText(mod->modInfo().name());
    ui->modSummary->setText(mod->modInfo().summary());
    ui->modAuthors->setText("by <b>" + mod->modInfo().author() + "</b>");
    ui->downloadSpeedText->setText(numberConvert(mod->modInfo().downloadCount(), "", 3, 1000) + tr(" Downloads"));

//    mod->acquireFullInfo();
//    connect(mod, &ModrinthMod::fullInfoReady, mod, &ModrinthMod::acquireFileList);

    connect(mod, &ModrinthMod::iconReady, this, &ModrinthModItemWidget::updateIcon);
    connect(mod, &ModrinthMod::fileListReady, this, &ModrinthModItemWidget::updateFileList);

}

ModrinthModItemWidget::~ModrinthModItemWidget()
{
    delete ui;
}

void ModrinthModItemWidget::updateIcon()
{
    QPixmap pixelmap;
    pixelmap.loadFromData(mod_->modInfo().iconBytes());
    ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
}

void ModrinthModItemWidget::updateFileList()
{
    auto menu = new QMenu(this);

    for(const auto &fileInfo : mod_->modInfo().featuredFileList()){
        auto name = fileInfo.displayName() + " ("+ numberConvert(fileInfo.size(), "B") + ")";
        connect(menu->addAction(name), &QAction::triggered, this, [=]{
//            downloadFile(fileInfo);
        });
    }

    ui->downloadButton->setMenu(menu);
}
