#include "optifinemodbrowser.h"
#include "ui_optifinemodbrowser.h"

#include <QScrollBar>
#include <QDebug>

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
    api_(new OptifineAPI(this)),
    bmclapi_(new BMCLAPI(this))
{
    ui->setupUi(this);
    ui->modListWidget->setVerticalScrollBar(new SmoothScrollBar(this));

    getModList();

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &OptifineModBrowser::updateLocalPathList);
    connect(ui->showPreview, &QCheckBox::stateChanged, this, &OptifineModBrowser::filterList);
    connect(ui->versionSelect, &QComboBox::currentTextChanged, this, &OptifineModBrowser::filterList);
    connect(ui->searchText, &QLineEdit::textChanged, this, &OptifineModBrowser::filterList);
    isUiSet_ = true;
}

OptifineModBrowser::~OptifineModBrowser()
{
    delete ui;
}

void OptifineModBrowser::refresh()
{
    getModList();
}

void OptifineModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    isUiSet_ = false;
    ui->versionSelect->setCurrentText(info.gameVersion());
    isUiSet_ = true;
    ui->downloadPathSelect->setCurrentText(info.displayName());
    filterList();
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
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = ui->modListWidget->item(i);
        if(!item->text().isEmpty()) continue;
        auto widget = ui->modListWidget->itemWidget(item);
        auto mod = dynamic_cast<const OptifineModItemWidget*>(widget)->mod();
        item->setHidden((gameVersion != GameVersion::Any && mod->modInfo().gameVersion() != gameVersion) ||
                        (!showPreview && mod->modInfo().isPreview()) ||
                        !(mod->modInfo().name().toLower().contains(searchText) || mod->modInfo().gameVersion().toString().contains(searchText)));
    }
}

void OptifineModBrowser::getModList()
{
    auto callback = [=](const auto &list){
        ui->modListWidget->clear();
        QStringList gameVersions;
        for(auto modInfo : list){
            if(!gameVersions.contains(modInfo.gameVersion()))
                gameVersions << modInfo.gameVersion();
            auto mod = new OptifineMod(this, modInfo);
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(0, 108));
            ui->modListWidget->addItem(listItem);
            auto itemWidget = new OptifineModItemWidget(this, mod);
            itemWidget->setDownloadPath(downloadPath_);
            connect(this, &OptifineModBrowser::downloadPathChanged, itemWidget, &OptifineModItemWidget::setDownloadPath);
            ui->modListWidget->setItemWidget(listItem, itemWidget);
            listItem->setHidden(modInfo.isPreview());
        }
        ui->versionSelect->clear();
        ui->versionSelect->addItem(tr("Any"));
        ui->versionSelect->addItems(gameVersions);

        auto item = new QListWidgetItem(tr("There is no more mod here..."));
        item->setSizeHint(QSize(0, 108));
        auto font = qApp->font();
        font.setPointSize(20);
        item->setFont(font);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->modListWidget->addItem(item);
    };
    auto source = Config().getOptifineSource();
    if(source == Config::OptifineSourceType::Official)
        api_->getModList(callback);
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
    if(!isUiSet_ || index < 0 || index >= ui->downloadPathSelect->count()) return;
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
