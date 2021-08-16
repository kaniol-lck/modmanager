#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>

#include "util/tutil.hpp"
#include "curseforgemod.h"
#include "curseforgemoditemwidget.h"
#include "curseforgemodinfodialog.h"
#include "gameversion.h"

CurseforgeModBrowser::CurseforgeModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeModBrowser)
{
    ui->setupUi(this);
    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &CurseforgeModBrowser::onSliderChanged);
}

CurseforgeModBrowser::~CurseforgeModBrowser()
{
    delete ui;
}

void CurseforgeModBrowser::updateVersions()
{
    ui->versionSelect->clear();
    ui->versionSelect->addItem(tr("Any"));
    for(const auto &version : qAsConst(GameVersion::versionList))
        ui->versionSelect->addItem(version);
}

void CurseforgeModBrowser::on_searchButton_clicked()
{
    currentName = ui->searchText->text();
    getModList(currentName);
}

void CurseforgeModBrowser::downloadFinished(QNetworkReply *reply)
{
    ui->searchButton->setText(tr("&Search"));
    ui->searchButton->setEnabled(true);

    //new search
    if(currentIndex == 0){
        modList.clear();
        ui->modListWidget->clear();
    }

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
    auto resultList = jsonDocument.toVariant().toList();

    for(const auto &entry : qAsConst(resultList)){
        auto curseforgeMod = CurseforgeMod::fromVariant(this, entry);
        curseforgeMod->downloadThumbnail();
        modList.append(curseforgeMod);

        auto *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(500, 100));
        auto modItemWidget = new CurseforgeModItemWidget(ui->modListWidget, curseforgeMod);

        ui->modListWidget->addItem(listItem);
        ui->modListWidget->setItemWidget(listItem, modItemWidget);
    }
}

void CurseforgeModBrowser::onSliderChanged(int i)
{
    if(i == ui->modListWidget->verticalScrollBar()->maximum()){
        currentIndex += 20;
        getModList(currentName, currentIndex);
    }
}

void CurseforgeModBrowser::getModList(QString name, int index)
{
    if(!index) currentIndex = 0;
    ui->searchButton->setText(tr("Searching..."));
    ui->searchButton->setEnabled(false);
    QUrl url("https://addons-ecs.forgesvc.net/api/v2/addon/search");

    //url query
    QUrlQuery urlQuery;

    //?
    urlQuery.addQueryItem("categoryId", "0");
    //minecraft
    urlQuery.addQueryItem("gameId", "432");
    //game version
    if(ui->versionSelect->currentIndex())
        urlQuery.addQueryItem("gameVersion", ui->versionSelect->currentText());
    //index
    urlQuery.addQueryItem("index", QString::number(index));
    //search page size, 20 by default [Customize it]
    urlQuery.addQueryItem("pageSize", "20");
    //search by name
    urlQuery.addQueryItem("searchFilter", name);
    //mod
    urlQuery.addQueryItem("sectionId", "6");
    //sort, 0 for no sort spec
    urlQuery.addQueryItem("sort", QString::number(ui->sortSelect->currentIndex()));

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto accessManager = new QNetworkAccessManager(this);
    connect(accessManager, &QNetworkAccessManager::finished, this, &CurseforgeModBrowser::downloadFinished);
    accessManager->get(request);
}

void CurseforgeModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = modList.at(index.row());
    auto dialog = new CurseforgeModInfoDialog(this, mod);
    dialog->show();
}


void CurseforgeModBrowser::on_versionSelect_currentIndexChanged(int)
{
    getModList(currentName);
}


void CurseforgeModBrowser::on_sortSelect_currentIndexChanged(int)
{
    getModList(currentName);
}

