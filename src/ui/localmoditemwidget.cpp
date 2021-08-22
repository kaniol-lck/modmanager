#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>

#include "local/localmodinfo.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "curseforgemodinfodialog.h"
#include "util/tutil.hpp"

LocalModItemWidget::LocalModItemWidget(QWidget *parent, LocalMod *mod) :
    QWidget(parent),
    ui(new Ui::LocalModItemWidget),
    localMod(mod)
{
    ui->setupUi(this);
    ui->modName->setText(mod->getModInfo().getName());
    ui->modVersion->setText(mod->getModInfo().getVersion());
    ui->modDescription->setText(mod->getModInfo().getDescription());

    if(!mod->getModInfo().getIconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(mod->getModInfo().getIconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

void LocalModItemWidget::needUpdate()
{
    ui->updateButton->setEnabled(true);
}

void LocalModItemWidget::on_updateButton_clicked()
{
    //TODO: update
}

