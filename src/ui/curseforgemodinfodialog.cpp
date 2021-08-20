#include "curseforgemodinfodialog.h"
#include "ui_curseforgemodinfodialog.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "curseforge/curseforgemod.h"
#include "ui/curseforgefileitemwidget.h"

CurseforgeModInfoDialog::CurseforgeModInfoDialog(QWidget *parent, CurseforgeMod *mod) :
    QDialog(parent),
    ui(new Ui::CurseforgeModInfoDialog),
    curseforgeMod(mod),
    accessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle(mod->getModInfo().getName());

    ui->modName->setText(mod->getModInfo().getName());
    ui->modSummary->setText(mod->getModInfo().getSummary());
    ui->modUrl->setText(QString("<a href= \"%1\">%1</a>").arg(mod->getModInfo().getWebsiteUrl().toString()));
    ui->modAuthors->setText(mod->getModInfo().getAuthors().join(", ").prepend(tr("by ")));

    //update thumbnail
    auto updateThumbnail = [=]{
        QPixmap pixelmap;
        pixelmap.loadFromData(curseforgeMod->getModInfo().getThumbnailBytes());
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80));
    };

    if(!curseforgeMod->getModInfo().getThumbnailBytes().isEmpty())
        updateThumbnail();
    else {
        connect(curseforgeMod, &CurseforgeMod::thumbnailReady, this, updateThumbnail);
    }

    //update description
    auto updateDescription = [=]{
        ui->modDescription->setText(curseforgeMod->getModInfo().getDescription());
    };

    if(!curseforgeMod->getModInfo().getDescription().isEmpty())
        updateDescription();
    else{
        mod->getDescription();
        connect(mod, &CurseforgeMod::descriptionReady, this, updateDescription);
    }

    //update file list
    auto updateFileList = [=]{
        ui->fileListWidget->clear();
        for(const auto &fileInfo : curseforgeMod->getModInfo().getAllFiles()){
            auto *listItem = new QListWidgetItem();
            listItem->setSizeHint(QSize(500, 90));
            auto itemWidget = new CurseforgeFileItemWidget(this, fileInfo);
            ui->fileListWidget->addItem(listItem);
            ui->fileListWidget->setItemWidget(listItem, itemWidget);
        }
    };

    if(!curseforgeMod->getModInfo().getAllFiles().isEmpty())
        updateFileList();
    else {
        mod->getAllFileList();
        connect(mod, &CurseforgeMod::allFileListReady, this, updateFileList);
    }
}

CurseforgeModInfoDialog::~CurseforgeModInfoDialog()
{
    delete ui;
}
