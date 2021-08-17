#include "curseforgemodbrowser.h"
#include "ui_curseforgemodbrowser.h"

#include <QScrollBar>
#include <QUrlQuery>
#include <QNetworkAccessManager>
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
    ui(new Ui::CurseforgeModBrowser),
    accessManager(new QNetworkAccessManager(this))
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
    urlQuery.addQueryItem("pageSize", "30");
    //search by name
    urlQuery.addQueryItem("searchFilter", name);
    //mod
    urlQuery.addQueryItem("sectionId", "6");
    //sort, 0 for no sort spec
    urlQuery.addQueryItem("sort", QString::number(ui->sortSelect->currentIndex()));

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    auto reply = accessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]{
        ui->searchButton->setText(tr("&Search"));
        ui->searchButton->setEnabled(true);

        //new search
        if(currentIndex == 0){
            for(auto mod : qAsConst(modList))
                mod->deleteLater();
            modList.clear();
            for(int i = 0; i < ui->modListWidget->count(); i++)
                ui->modListWidget->itemWidget(ui->modListWidget->item(i))->deleteLater();
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
            auto curseforgeMod = CurseforgeMod::fromVariant(this, accessManager, entry);
            modList.append(curseforgeMod);

            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 100));
            auto modItemWidget = new CurseforgeModItemWidget(ui->modListWidget, curseforgeMod);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            setItemHidden(listItem, curseforgeMod);
            if(!listItem->isHidden())
                curseforgeMod->downloadThumbnail();
        }
    });
}

void CurseforgeModBrowser::setItemHidden(QListWidgetItem *item, const CurseforgeMod *mod)
{
    int index = ui->loaderSelect->currentIndex();
    switch (index) {
    //any
    case 0:
        item->setHidden(false);
        break;
    //fabric
    case 1:
        item->setHidden(!mod->isFabricMod());
        break;
    //forge
    case 2:
        item->setHidden(!mod->isForgeMod());
    //rift
    case 3:
        item->setHidden(!mod->isRiftMod());
    }
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


void CurseforgeModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        auto b = item->isHidden();
        auto mod = modList.at(i);
        setItemHidden(item, mod);
        if(b && !item->isHidden() && mod->getThumbnailBytes().isEmpty())
            mod->downloadThumbnail();
    }
}

