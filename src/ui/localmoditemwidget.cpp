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

LocalModItemWidget::LocalModItemWidget(QWidget *parent, QNetworkAccessManager *manager, const LocalModInfo &modInfo) :
    QWidget(parent),
    ui(new Ui::LocalModItemWidget),
    localModInfo(modInfo),
    accessManager(manager)
{
    ui->setupUi(this);
    ui->modName->setText(modInfo.getName());
    ui->modVersion->setText(modInfo.getVersion());
    ui->modDescription->setText(modInfo.getDescription());

    if(!modInfo.getIconBytes().isEmpty()){
        QPixmap pixelmap;
        pixelmap.loadFromData(modInfo.getIconBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    }
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

CurseforgeMod *LocalModItemWidget::getCurseforgeMod() const
{
    return curseforgeMod;
}
