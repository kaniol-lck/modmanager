#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>

#include "localmodinfo.h"
#include "curseforgemod.h"
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

void LocalModItemWidget::searchOnCurseforge()
{
    auto accessManager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://addons-ecs.forgesvc.net/api/v2/fingerprint"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto reply = accessManager->post(request, QString("[ %1 ]").arg(localModInfo.getMurmurhash()).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [=]{
        if(reply->error() != QNetworkReply::NoError) return ;

        QJsonParseError error;
        auto json = QJsonDocument::fromJson(reply->readAll(), &error);
        if(error.error != QJsonParseError::NoError) return;

        auto exactMatchList = value(json.toVariant(), "exactMatches").toList();

        if(exactMatchList.isEmpty()) return;
        auto result = exactMatchList.at(0);
        curseforgeMod = CurseforgeMod::fromVariant(this, accessManager, result);
        ui->curseforgeButton->setEnabled(true);
        emit curseforgeModGot();
        reply->deleteLater();
    });
}

CurseforgeMod *LocalModItemWidget::getCurseforgeMod() const
{
    return curseforgeMod;
}

void LocalModItemWidget::on_curseforgeButton_clicked()
{
    auto dialog = new CurseforgeModInfoDialog(this, curseforgeMod);
    dialog->show();
}

