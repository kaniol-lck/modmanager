#include "optifinemodbrowser.h"
#include "ui_optifinemodbrowser.h"

#include <QScrollBar>
#include <QDebug>
#include <QStandardItem>

#include "optifinemoditemwidget.h"
#include "optifine/optifineapi.h"
#include "optifine/optifinemod.h"
#include "curseforge/curseforgemod.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "bmclapi.h"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"
#include "config.hpp"
#include "util/smoothscrollbar.h"

OptifineModBrowser::OptifineModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/optifine.png"), "OptiFine", QUrl("https://www.optifine.net")),
    ui(new Ui::OptifineModBrowser),
    model_(new QStandardItemModel(this)),
    api_(new OptifineAPI(this)),
    bmclapi_(new BMCLAPI(this))
{
    ui->setupUi(this);
    ui->modListView->setModel(model_);
    ui->modListView->setVerticalScrollBar(new SmoothScrollBar(this));
    ui->modListView->setProperty("class", "ModList");

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &OptifineModBrowser::updateLocalPathList);
    connect(ui->showPreview, &QCheckBox::stateChanged, this, &OptifineModBrowser::filterList);
    connect(ui->versionSelect, &QComboBox::currentTextChanged, this, &OptifineModBrowser::filterList);
    connect(ui->searchText, &QLineEdit::textChanged, this, &OptifineModBrowser::filterList);
    connect(ui->modListView->verticalScrollBar(), &QScrollBar::valueChanged, this, &OptifineModBrowser::updateIndexWidget);

    if(Config().getSearchModsOnStartup()){
        inited_ = true;
        getModList();
    }
}

OptifineModBrowser::~OptifineModBrowser()
{
    for(auto row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<OptifineMod*>();
        if(mod && !mod->parent())
            mod->deleteLater();
    }
    delete ui;
}

void OptifineModBrowser::refresh()
{
    getModList();
}

void OptifineModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    ui->versionSelect->blockSignals(true);
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->versionSelect->blockSignals(false);
    ui->downloadPathSelect->setCurrentText(info.displayName());
    filterList();
}

void OptifineModBrowser::updateUi()
{

}

void OptifineModBrowser::updateLocalPathList()
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

void OptifineModBrowser::filterList()
{
    auto gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto showPreview = ui->showPreview->isChecked();
    auto searchText = ui->searchText->text().toLower();
    for(int row = 0; row < model_->rowCount(); row++){
        auto item = model_->item(row);
        auto mod = item->data().value<OptifineMod*>();
        if(mod){
            ui->modListView->setRowHidden(row, (gameVersion != GameVersion::Any && mod->modInfo().gameVersion() != gameVersion) ||
                                          (!showPreview && mod->modInfo().isPreview()) ||
                                          !(mod->modInfo().name().toLower().contains(searchText) || mod->modInfo().gameVersion().toString().contains(searchText)));
        }
    }
}

void OptifineModBrowser::getModList()
{
    auto callback = [=](const auto &list){
        for(auto row = 0; row < model_->rowCount(); row++){
            auto item = model_->item(row);
            auto mod = item->data().value<OptifineMod*>();
            if(mod && !mod->parent())
                mod->deleteLater();
        }
        model_->clear();
        QStringList gameVersions;
        for(auto modInfo : list){
            if(!gameVersions.contains(modInfo.gameVersion()))
                gameVersions << modInfo.gameVersion();
            auto mod = new OptifineMod(nullptr, modInfo);
            auto item = new QStandardItem;
            item->setData(QVariant::fromValue(mod));
            model_->appendRow(item);
            item->setSizeHint(QSize(0, 100));
            ui->modListView->setRowHidden(item->row(), modInfo.isPreview());
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
    };
    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official)
        connect(this, &QObject::destroyed, disconnecter(
                    api_->getModList(callback)));
    else if (source == Config::OptifineSourceType::BMCLAPI)
        bmclapi_->getOptifineList(callback);
}

void OptifineModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void OptifineModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void OptifineModBrowser::on_getOptiFabric_clicked()
{
    //OptiFabric on Curseforge
    //Project ID 322385
    auto mod = new CurseforgeMod(this, 322385);
    auto dialog = new CurseforgeModDialog(this, mod);
    dialog->setDownloadPath(downloadPath_);
    connect(this, &OptifineModBrowser::downloadPathChanged, dialog, &CurseforgeModDialog::setDownloadPath);
    dialog->show();
}

void OptifineModBrowser::on_getOptiForge_clicked()
{
    //OptiForge on Curseforge
    //Project ID 372196
    auto mod = new CurseforgeMod(this, 372196);
    auto dialog = new CurseforgeModDialog(this, mod);
    dialog->setDownloadPath(downloadPath_);
    connect(this, &OptifineModBrowser::downloadPathChanged, dialog, &CurseforgeModDialog::setDownloadPath);
    dialog->show();
}

void OptifineModBrowser::updateIndexWidget()
{
    auto beginRow = ui->modListView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->modListView->indexAt(QPoint(0, ui->modListView->height())).row();
    //extra 2
    endRow += 2;
    for(int row = beginRow; row <= endRow && row < model_->rowCount(); row++){
        auto index = model_->index(row, 0);
        if(ui->modListView->indexWidget(index)) continue;
        auto item = model_->item(row);
        auto mod = item->data().value<OptifineMod*>();
        if(mod){
            auto modItemWidget = new OptifineModItemWidget(this, mod);
            modItemWidget->setDownloadPath(downloadPath_);
            connect(this, &OptifineModBrowser::downloadPathChanged, modItemWidget, &OptifineModItemWidget::setDownloadPath);
            ui->modListView->setIndexWidget(index, modItemWidget);
        }
    }
}

void OptifineModBrowser::paintEvent(QPaintEvent *event)
{
    if(!inited_){
        inited_ = true;
        getModList();
    }
    updateIndexWidget();
    QWidget::paintEvent(event);
}
