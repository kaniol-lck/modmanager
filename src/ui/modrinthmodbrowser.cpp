#include "modrinthmodbrowser.h"
#include "ui_modrinthmodbrowser.h"

#include <QScrollBar>

#include "modrinthmoditemwidget.h"
#include "modrinthmodinfodialog.h"
#include "modrinth/modrinthapi.h"
#include "gameversion.h"
#include "modloadertype.h"

ModrinthModBrowser::ModrinthModBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModrinthModBrowser)
{
    ui->setupUi(this);

    connect(ui->modListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged,  this , &ModrinthModBrowser::onSliderChanged);

    getModList(currentName);
}

ModrinthModBrowser::~ModrinthModBrowser()
{
    delete ui;
}

void ModrinthModBrowser::on_searchButton_clicked()
{
    currentName = ui->searchText->text();
    getModList(currentName);
}

void ModrinthModBrowser::onSliderChanged(int i)
{
    if(hasMore && i == ui->modListWidget->verticalScrollBar()->maximum()){
        currentIndex += 20;
        getModList(currentName, currentIndex);
    }
}

void ModrinthModBrowser::getModList(QString name, int index)
{
    if(!index) currentIndex = 0;
    ui->searchButton->setText(tr("Searching..."));
    ui->searchButton->setEnabled(false);
    setCursor(Qt::BusyCursor);

    GameVersion gameVersion = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()) : GameVersion::ANY;
    auto sort = ui->sortSelect->currentIndex();

    ModrinthAPI::searchMods(name, currentIndex, sort, [=](const QList<ModrinthModInfo> &infoList){
        ui->searchButton->setText(tr("&Search"));
        ui->searchButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);

        //new search
        if(currentIndex == 0){
            for(auto mod : qAsConst(modList))
                mod->deleteLater();
            modList.clear();
            for(int i = 0; i < ui->modListWidget->count(); i++)
                ui->modListWidget->itemWidget(ui->modListWidget->item(i))->deleteLater();
            ui->modListWidget->clear();
            hasMore = true;
        }

        if(infoList.isEmpty()){
            hasMore = false;
            return ;
        }

        //show them
        for(const auto &info : qAsConst(infoList)){
            auto modrinthMod = new ModrinthMod(this, info);
            modList.append(modrinthMod);

            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 100));
            auto version = ui->versionSelect->currentIndex()? GameVersion(ui->versionSelect->currentText()): GameVersion::ANY;
            auto modItemWidget = new ModrinthModItemWidget(ui->modListWidget, modrinthMod);
            ui->modListWidget->addItem(listItem);
            ui->modListWidget->setItemWidget(listItem, modItemWidget);
            modrinthMod->acquireIcon();
        }
    });
}


void ModrinthModBrowser::on_modListWidget_doubleClicked(const QModelIndex &index)
{
    auto mod = modList.at(index.row());
    auto dialog = new ModrinthModInfoDialog(this, mod);
    dialog->show();
}

