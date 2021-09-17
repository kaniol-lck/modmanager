#include "curseforgemodinfodialog.h"
#include "ui_curseforgemodinfodialog.h"

#include "curseforge/curseforgemod.h"
#include "ui/curseforgefileitemwidget.h"
#include "util/datetimesortitem.h"

CurseforgeModInfoDialog::CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod, const QString &path, LocalMod *localMod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModInfoDialog),
    mod_(mod),
    localMod_(localMod),
    downloadPath_(path)
{
    ui->setupUi(this);

    //update basic info
    auto updateBasicInfo = [=]{
        setWindowTitle(mod->modInfo().name() + tr(" - Curseforge"));
        ui->modName->setText(mod->modInfo().name());
        ui->modSummary->setText(mod->modInfo().summary());
        ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->modInfo().websiteUrl().toString()));
        ui->modAuthors->setText(mod->modInfo().authors().join(", ").prepend(tr("by ")));

        //update thumbnail
        //included by basic info
        auto updateThumbnail = [=]{
            QPixmap pixelmap;
            pixelmap.loadFromData(mod_->modInfo().iconBytes());
            ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
            ui->modIcon->setCursor(Qt::ArrowCursor);
        };

        if(!mod_->modInfo().iconBytes().isEmpty())
            updateThumbnail();
        else {
            mod->acquireIcon();
            ui->modIcon->setCursor(Qt::BusyCursor);
            connect(mod_, &CurseforgeMod::iconReady, this, updateThumbnail);
        }
    };

    if(mod_->modInfo().hasBasicInfo())
        updateBasicInfo();
    else {
        mod->acquireBasicInfo();
        connect(mod_, &CurseforgeMod::basicInfoReady, this, updateBasicInfo);
    }

    //update description
    auto updateDescription = [=]{
        ui->modDescription->setFont(qApp->font());
        ui->modDescription->setHtml(mod_->modInfo().description());
        ui->modDescription->setCursor(Qt::ArrowCursor);
    };

    if(!mod_->modInfo().description().isEmpty())
        updateDescription();
    else{
        ui->modDescription->setCursor(Qt::BusyCursor);
        mod->acquireDescription();
        connect(mod, &CurseforgeMod::descriptionReady, this, updateDescription);
    }

    //update file list
    auto updateFileList = [=]{
        ui->fileListWidget->clear();
        auto files = mod_->modInfo().allFileList();
        for(const auto &fileInfo : files){
            auto *listItem = new DateTimeSortItem();
            listItem->setData(DateTimeSortItem::Role, fileInfo.fileDate());
            listItem->setSizeHint(QSize(500, 90));
            auto itemWidget = new CurseforgeFileItemWidget(this, mod_, fileInfo, downloadPath_, localMod_);
            connect(this, &CurseforgeModInfoDialog::downloadPathChanged, itemWidget, &CurseforgeFileItemWidget::setDownloadPath);
            ui->fileListWidget->addItem(listItem);
            ui->fileListWidget->setItemWidget(listItem, itemWidget);
        }
        ui->fileListWidget->sortItems(Qt::DescendingOrder);
        ui->fileListWidget->setCursor(Qt::ArrowCursor);
    };

    if(!mod_->modInfo().allFileList().isEmpty())
        updateFileList();
    else {
        ui->fileListWidget->setCursor(Qt::BusyCursor);
        mod->acquireAllFileList();
        connect(mod, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}

CurseforgeModInfoDialog::~CurseforgeModInfoDialog()
{
    delete ui;
}

void CurseforgeModInfoDialog::setDownloadPath(const QString &newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    emit downloadPathChanged(newDownloadPath);
}
