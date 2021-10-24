#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>
#include <QDir>

#include "modrinthmoditemwidget.h"
#include "modrinthmoddialog.h"
#include "local/localmodpathmanager.h"
#include "local/localmodpath.h"
#include "modrinth/modrinthmod.h"
#include "modrinth/modrinthapi.h"
#include "gameversion.h"
#include "modloadertype.h"
#include "config.hpp"
#include "util/funcutil.h"
#include "util/smoothscrollbar.h"

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent) :
    ExploreBrowser(parent, QIcon(":/image/modrinth.svg"), "Modrinth", QUrl("https://modrinth.com/mods")),
    ui(new Ui::ModrinthModBrowser),
    api_(new ModrinthAPI(this))
{
    ui->setupUi(this);
    ui->modListWidget->setVerticalScrollBar(new SmoothScrollBar(this));

    for(const auto &type : ModLoaderType::modrinth)
        ui->loaderSelect->addItem(ModLoaderType::icon(type), ModLoaderType::toString(type));

    //categories
    //TODO: using menu/submenu
    ui->categorySelect->clear();
    ui->categorySelect->addItem(tr("Any"));
    for(const auto &[name, iconName] : ModrinthAPI::getCategories()){
        QIcon icon(QString(":/image/modrinth/%1.svg").arg(iconName));
        ui->categorySelect->addItem(icon, name);
    }

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &ModrinthModBrowser::onSliderChanged);
    connect(ui->searchText, &QLineEdit::returnPressed, this, &ModrinthModBrowser::search);

    updateVersionList();
    connect(VersionManager::manager(), &VersionManager::modrinthVersionListUpdated, this, &ModrinthModBrowser::updateVersionList);

    updateLocalPathList();
    connect(LocalModPathManager::manager(), &LocalModPathManager::pathListUpdated, this, &ModrinthModBrowser::updateLocalPathList);

    getModList(currentName_);
    isUiSet_ = true;
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
}

void ModrinthModBrowser::refresh()
{
    search();
}

void ModrinthModBrowser::searchModByPathInfo(const LocalModPathInfo &info)
{
    isUiSet_ = false;
    ui->versionSelect->setCurrentText(info.gameVersion());
    ui->loaderSelect->setCurrentIndex(ModLoaderType::modrinth.indexOf(info.loaderType()));
    isUiSet_ = true;
    ui->downloadPathSelect->setCurrentText(info.displayName());
    getModList(currentName_);
}

void ModrinthModBrowser::updateVersionList()
{
    isUiSet_ = false;
    ui->versionSelect->clear();
    ui->versionSelect->addItem(tr("Any"));
    for(auto &&version : GameVersion::modrinthVersionList())
        ui->versionSelect->addItem(version);
    isUiSet_ = true;
}

void ModrinthModBrowser::updateLocalPathList()
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

void ModrinthModBrowser::search()
{
//    if(ui->searchText->text() == currentName_) return;
    currentName_ = ui->searchText->text();
    getModList(currentName_);
}

void ModrinthModBrowser::onSliderChanged(int i)
{
    if(!isSearching_ && hasMore_ && i >= ui->modListWidget->verticalScrollBar()->maximum() - 1000){
        currentIndex_ += Config().getSearchResultCount();
        getModList(currentName_, currentIndex_);
    }
}

void ModrinthModBrowser::getModList(QString name, int index)
{
    if(!index)
        currentIndex_ = 0;
    else if(!hasMore_ || isSearching_)
        return;
    setCursor(Qt::BusyCursor);

    GameVersion gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto categoryIndex = ui->categorySelect->currentIndex() - 1;
    auto category = categoryIndex < 0 ? "" : std::get<1>(ModrinthAPI::getCategories()[categoryIndex]);
    auto sort = ui->sortSelect->currentIndex();
    GameVersion version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::Any;
    auto type = ModLoaderType::modrinth.at(ui->loaderSelect->currentIndex());

    isSearching_ = true;
    api_->searchMods(name, currentIndex_, version, type, category, sort, [=](const QList<ModrinthModInfo> &infoList){
        setCursor(Qt::ArrowCursor);

        //new search
        if(currentIndex_ == 0){
            for(int i = 0; i < ui->modListWidget->count(); i++)
                ui->modListWidget->itemWidget(ui->modListWidget->item(i))->deleteLater();
            ui->modListWidget->clear();
            hasMore_ = true;
        }

        //show them
        for(const auto &info : qAsConst(infoList)){
            auto mod = new ModrinthMod(this, info);

            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(0, 108));
            auto version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()): GameVersion::Any;
            auto modItemWidget = new ModrinthModItemWidget(ui->modListWidget, mod);
            mod->setParent(modItemWidget);
            modItemWidget->setDownloadPath(downloadPath_);
            connect(this, &ModrinthModBrowser::downloadPathChanged, modItemWidget, &ModrinthModItemWidget::setDownloadPath);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            mod->acquireIcon();
        }
        if(infoList.size() < Config().getSearchResultCount()){
            auto item = new QListWidgetItem(tr("There is no more mod here..."));
            item->setSizeHint(QSize(0, 108));
            auto font = qApp->font();
            font.setPointSize(20);
            item->setFont(font);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->modListWidget->addItem(item);
            hasMore_ = false;
        }
        isSearching_ = false;
    });
}

void ModrinthModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto item = ui->modListWidget->item(index.row());
    if(item->data(Qt::UserRole + 1).toBool()) return;
    item->setData(Qt::UserRole + 1, true);
    if(!item->text().isEmpty()) return;
    auto widget = dynamic_cast<ModrinthModItemWidget*>(ui->modListWidget->itemWidget(item));
    auto mod = widget->mod();
    auto dialog = new ModrinthModDialog(this, mod);
    //set parent
    mod->setParent(dialog);
    dialog->setDownloadPath(downloadPath_);
    connect(this, &ModrinthModBrowser::downloadPathChanged, dialog, &ModrinthModDialog::setDownloadPath);
    connect(dialog, &ModrinthModDialog::finished, widget, [=]{
        mod->setParent(widget);
        item->setData(Qt::UserRole + 1, false);
    });
    dialog->show();
}

void ModrinthModBrowser::on_sortSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_versionSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_loaderSelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}

void ModrinthModBrowser::on_openFolderButton_clicked()
{
    QString path;
    if(downloadPath_)
        path = downloadPath_->info().path();
    else
        path = Config().getDownloadPath();
    openFileInFolder(path);
}

void ModrinthModBrowser::on_downloadPathSelect_currentIndexChanged(int index)
{
    if(!isUiSet_ || index < 0 || index >= ui->downloadPathSelect->count()) return;
    if(index == 0)
        downloadPath_ = nullptr;
    else
        downloadPath_ =  LocalModPathManager::pathList().at(index - 1);
    emit downloadPathChanged(downloadPath_);
}

void ModrinthModBrowser::on_categorySelect_currentIndexChanged(int)
{
    if(isUiSet_) getModList(currentName_);
}
