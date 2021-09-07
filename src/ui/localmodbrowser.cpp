#include "localmodbrowser.h"
#include "ui_localmodbrowser.h"


#include "local/localmodpath.h"
#include "localmoditemwidget.h"
#include "localmodinfodialog.h"
#include "localmodupdatedialog.h"
#include "config.h"
#include "util/localmodsortitem.h"

LocalModBrowser::LocalModBrowser(QWidget *parent, LocalModPath *modPath) :
    QWidget(parent),
    ui(new Ui::LocalModBrowser),
    modPath_(modPath)
{
    ui->setupUi(this);

    updateModList();
    connect(modPath_, &LocalModPath::modListUpdated, this, &LocalModBrowser::updateModList);
}

LocalModBrowser::~LocalModBrowser()
{
    delete ui;
}

void LocalModBrowser::updateModList()
{
    ui->modListWidget->clear();
    for (const auto &mod: modPath_->modList()) {
        auto modItemWidget = new LocalModItemWidget(ui->modListWidget, mod);

        //curseforge
        mod->searchOnCurseforge();
        connect(mod, &LocalMod::curseforgeReady, this, [=](bool bl){
            if(bl && Config().getAutoCheckUpdate()) mod->checkCurseforgeUpdate(modPath_->info().gameVersion(), modPath_->info().loaderType());
        });

        //modrinth
        mod->searchOnModrinth();
        connect(mod, &LocalMod::modrinthReady, this, [=](bool bl){
//            if(bl && Config().getAutoCheckUpdate()) localMod->checkModrinthUpdate(modDirInfo.getGameVersion(), modDirInfo.getLoaderType());
        });

        auto *item = new LocalModSortItem(mod);
        item->setSizeHint(QSize(500, 100));

        ui->modListWidget->addItem(item);
        ui->modListWidget->setItemWidget(item, modItemWidget);
    }
    ui->modListWidget->sortItems();
}

void LocalModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = dynamic_cast<const LocalModSortItem*>(ui->modListWidget->item(index.row()))->mod();
    auto dialog = new LocalModInfoDialog(this, mod);
    dialog->show();
}

void LocalModBrowser::on_updateAllButton_clicked()
{
    auto dialog = new LocalModUpdateDialog(this, modPath_);
    dialog->show();
}


void LocalModBrowser::on_searchText_textEdited(const QString &arg1)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto mod = dynamic_cast<const LocalModSortItem*>(ui->modListWidget->item(i))->mod();
        auto str = arg1.toLower();
        if(mod->modInfo().name().toLower().contains(str) ||
                mod->modInfo().description().toLower().contains(str))
            ui->modListWidget->item(i)->setHidden(false);
        else
            ui->modListWidget->item(i)->setHidden(true);
    }
}


void LocalModBrowser::on_comboBox_currentIndexChanged(int index)
{
    for(int i = 0; i < ui->modListWidget->count(); i++){
        auto item = dynamic_cast<LocalModSortItem*>(ui->modListWidget->item(i));
        item->setSortRule(static_cast<LocalModSortItem::SortRule>(index));
    }
    ui->modListWidget->sortItems();
}

