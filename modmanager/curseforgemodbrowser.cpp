#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>

#include "util/qjsonutil.hpp"
#include "curseforgemod.h"
#include "curseforgemoditemwidget.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModBrowser),
    accessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    connect(accessManager, &QNetworkAccessManager::finished, this, &CurseforgeModBrowser::downloadFinished);
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
}

void CurseforgeModBrowser::on_searchButton_clicked()
{
    ui->searchButton->setText(tr("Searching..."));
    ui->searchButton->setEnabled(false);
    QNetworkRequest request;
    QUrl url = QString("https://addons-ecs.forgesvc.net/api/v2/addon/search?categoryId=0&gameId=432&index=0&pageSize=20&searchFilter=%1&sectionId=6&sort=0")
            .arg(ui->searchText->text());

    request.setUrl(url);
    accessManager->get(request);
}

void CurseforgeModBrowser::downloadFinished(QNetworkReply *reply)
{
    ui->searchButton->setText(tr("&Search"));
    ui->searchButton->setEnabled(true);
    modList.clear();
    ui->modListWidget->clear();

    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        return;
    }

    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return;
    }
    QVariant result = jsonDocument.toVariant();

    for(const auto &entry : result.toList()){
        auto curseforgeMod = CurseforgeMod::fromVariant(this, entry);
        modList.append(curseforgeMod);

        qDebug()<<curseforgeMod->getName();

        auto *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(500, 100));
        auto modEntryWidget = new CurseforgeModItemWidget(ui->modListWidget, curseforgeMod);

        ui->modListWidget->addItem(listItem);
        ui->modListWidget->setItemWidget(listItem, modEntryWidget);
    }
}

