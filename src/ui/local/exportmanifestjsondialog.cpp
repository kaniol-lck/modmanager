#include "exportmanifestjsondialog.h"
#include "ui_exportmanifestjsondialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "local/localfilelinker.h"
#include "curseforge/curseforgemod.h"
#include "util/funcutil.h"

#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

ExportManifestJsonDialog::ExportManifestJsonDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::ExportManifestJsonDialog),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->modTreeView->setModel(&model_);
    ui->modTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);

    for(auto &&gameVersion : GameVersion::mojangVersionList())
        ui->gameVersion->addItem(gameVersion);
    ui->gameVersion->setCurrentText(modPath_->info().gameVersion());

    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(IdColumn, new QStandardItem(tr("Curseforge Project ID")));
    model_.setHorizontalHeaderItem(FileIdColumn, new QStandardItem(tr("Curseforge File ID")));
    model_.setHorizontalHeaderItem(RequiredColumn, new QStandardItem(tr("Required")));

    for(auto &&map : modPath_->modMaps()) for(const auto &mod : map){
        auto nameItem = new QStandardItem;
        nameItem->setData(QVariant::fromValue(mod));
        auto idItem = new QStandardItem;
        auto fileIdItem = new QStandardItem;
        auto requiredItem = new QStandardItem;
        auto list = {nameItem, idItem, fileIdItem, requiredItem};
        model_.appendRow(list);

        nameItem->setText(mod->commonInfo()->name());
        nameItem->setIcon(mod->icon());
        nameItem->setCheckable(true);
        nameItem->setCheckState(Qt::Checked);
        requiredItem->setCheckable(true);
        requiredItem->setCheckState(Qt::Checked);

        if(auto curseforgeMod = mod->curseforgeMod())
            idItem->setText(QString::number(curseforgeMod->modInfo().id()));
        if(auto fileInfo = mod->modFile()->linker()->curseforgeFileInfo())
            fileIdItem->setText(QString::number(fileInfo->id()));

        if(idItem->text().isEmpty() || fileIdItem->text().isEmpty()){
            requiredItem->setCheckable(false);
            disabledMods_ << mod;
            nameItem->setCheckState(Qt::Unchecked);
            for(auto item : list)
                item->setEnabled(false);
        }
        if(mod->isDisabled())
            nameItem->setCheckState(Qt::Unchecked);
    }

    if(!disabledMods_.isEmpty()){
        ui->disabledMods->setText(tr("%1 mods disabled exporting,"
                                     "because they have no corresponding project ID or file ID on Curseforge.")
                                  .arg(disabledMods_.size()));
        QStringList stringList;
        for(auto mod : qAsConst(disabledMods_))
            stringList << mod->displayName();
        ui->disabledMods->setToolTip(stringList.join(tr(", ")));
    }
}

ExportManifestJsonDialog::~ExportManifestJsonDialog()
{
    delete ui;
}

void ExportManifestJsonDialog::on_buttonBox_accepted()
{
    QJsonObject object;
    object.insert("manifestType", "minecraftModpack");
    object.insert("manifestVersion", 1);
    object.insert("name", ui->name->text());
    object.insert("author", ui->author->text());
    QJsonObject minecraftObject;
    minecraftObject.insert("version", ui->gameVersion->currentText());
    object.insert("minecraft", minecraftObject);
    QJsonArray filesArray;
    for(int row = 0; row < model_.rowCount(); row++)
        if(auto item = model_.item(row); item->checkState() == Qt::Checked){
            QJsonObject fileObject;
            fileObject.insert("projectID", model_.item(row, IdColumn)->text());
            fileObject.insert("fileID", model_.item(row, FileIdColumn)->text());
            fileObject.insert("required", model_.item(row, RequiredColumn)->checkState() == Qt::Checked);
            filesArray << fileObject;
        }
    object.insert("files", filesArray);

    auto filePath = QFileDialog::getSaveFileName(this, tr("Save your manifest.json"),
                                                  QDir(modPath_->info().path()).filePath("manifest.json"), "*.json");
    if(filePath.isEmpty()) return;
    QJsonDocument doc(object);
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, tr("Failed Save"), tr("Can not write to file."));
        return;
    }
    file.write(doc.toJson());
    openFileInFolder(filePath);
    accept();
}

