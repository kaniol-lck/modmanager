#include "replaymodbrowser.h"
#include "ui_replaymodbrowser.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScrollBar>
#include <QStandardItem>

#include "replaymoditemwidget.h"
#include "replay/replayapi.h"
#include "replay/replaymod.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"
#include "config.hpp"

ReplayModBrowser::ReplayModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/replay.png"), "ReplayMod", QUrl("https://www.replaymod.com")),
    ui(new Ui::ReplayModBrowser),
    model_(new QStandardItemModel(this)),
    api_(new ReplayAPI())
{
    ui->setupUi(this);
    ui->modListView->setModel(model_);
    ui->modListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListView->setProperty("class", "ModList");

    for(const auto &type : ModLoaderType::replay)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ReplayModBrowser::updateLocalPathList);

    connect(ui->versionSelect, &QComboBox::currentTextChanged, this, &ReplayModBrowser::filterList);
    connect(ui->loaderSelect, &QComboBox::currentTextChanged, this, &ReplayModBrowser::filterList);
    connect(ui->searchText, &QLineEdit::textChanged, this, &ReplayModBrowser::filterList);
    connect(ui->modListView->verticalScrollBar(), &QScrollBar::valueChanged, this, &ReplayModBrowser::updateIndexWidget);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList();
    }
}

ReplayModBrowser::~ReplayModBrowser()
{
    delete ui;
}

void ReplayModBrowser::refresh()
{
    getModList();
}

void ReplayModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::curseforge.indexOf(info.loaderType()));
    ui->downloadPathSelect->setCurrentText(info.displayName());
    filterList();
}

void ReplayModBrowser::updateUi()
{

}

void ReplayModBrowser::updateLocalPathList()
{
    //remember selected path
    LocalModPath *selectedPath = nullptr;
    auto index = ui->downloadPathSelect->currentIndex();
    if(index >= 0 && index < LocalModPathManager::pathList().size())
        selectedPath = LocalModPathManager::pathList().at(ui->downloadPathSelect->currentIndex());

    ui->downloadPathSelect->clear();
    ui->downloadPathSelect->addItem(tr("Custom"));
    for(const auto &path : LocalModPathManager::pathList())
        ui->downloadPathSelect->addItem(path->info().displayName());

    //reset selected path
    if(selectedPath != nullptr){
        auto index = LocalModPathManager::pathList().indexOf(selectedPath);
        if(index >= 0)
            ui->downloadPathSelect->setCurrentIndex(index);
    }
}

void ReplayModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void ReplayModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void ReplayModBrowser::updateIndexWidget()
{
    auto beginRow = ui->modListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->modListView->indexAt(QPoint(0, ui->modListView->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, 0);
        if(ui->modListView->indexWidget(index)) continue;
        auto item = model_->item(row);
        auto mod = item->data().value<ReplayMod*>();
        if(mod){
            auto modItemWidget = new ReplayModItemWidget(this, mod);
            modItemWidget->setDownloadPath(downloadPath_);
            connect(this, &ReplayModBrowser::downloadPathChanged, modItemWidget, &ReplayModItemWidget::setDownloadPath);
            ui->modListView->setIndexWidget(index, modItemWidget);
        }
    }
}

void ReplayModBrowser::paintEvent(QPaintEvent *event)
{
    if(!inited_){
        inited_ = true;
        getModList();
    }
    updateIndexWidget();
    QWidget::paintEvent(event);
}

void ReplayModBrowser::getModList()
{
    api_->getModList([=](const auto &list){
        for(auto row = 0; row < model_->rowCount(); row++){
            auto item = model_->item(row);
            auto mod = item->data().value<ReplayMod*>();
            if(mod && !mod->parent())
                mod->deleteLater();
        }
        model_->clear();
        QStringList gameVersions;
        for(auto modInfo : list){
            if(!gameVersions.contains(modInfo.gameVersion()))
                gameVersions << modInfo.gameVersion();
            auto mod = new ReplayMod(this, modInfo);
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
        }
        ui->versionSelect->clear();
        ui->versionSelect->addItem(tr("Any"));
        ui->versionSelect->addItems(gameVersions);

        auto item = new QStandardItem(tr("There is no more mod here..."));
        item->setSizeHint(QSize(0, 108));
        auto font = qApp->font();
        font.setPointSize(20);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model_->appendRow(item);
    });
}

void ReplayModBrowser::filterList()
{
    auto gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto loaderType = ModLoaderType::curseforge.at(ui->loaderSelect->currentIndex());
    auto searchText = ui->searchText->text().toLower();
    for(int row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<ReplayMod*>();
        if(mod){
            ui->modListView->setRowHidden(row, (loaderType != ModLoaderType::Any && mod->modInfo().loaderType() != loaderType) ||
                    (gameVersion != GameVersion::Any && gameVersion != mod->modInfo().gameVersion()) ||
                    !(mod->modInfo().name().toLower().contains(searchText) ||
                      mod->modInfo().gameVersionString().contains(searchText)));
        }
    }
}
