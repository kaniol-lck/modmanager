#include "exportmanifestjsondialog.h"
#include "ui_exportmanifestjsondialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "local/localfilelinker.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/funcutil.h"

#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

ExportManifestJsonDialog::ExportManifestJsonDialog(QWidget *parent, LocalModPath *modPath, Mode mode) :
    QDialog(parent),
    ui(new Ui::ExportManifestJsonDialog),
    modPath_(modPath),
    mode_(mode)
{
    ui->setupUi(this);
    ui->modTreeView->setModel(&model_);
    ui->modTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);

    QString disableReason;
    switch (mode_) {
    case ExportManifestJsonDialog::MODPACKS:
    case ExportManifestJsonDialog::MANIFEST_JSON:{
        if(mode_ == MODPACKS)
            setWindowTitle("Export Modpacks");
        else
            setWindowTitle(tr("Export manifest.json"));

        for(auto &&gameVersion : GameVersion::mojangVersionList())
            ui->gameVersion->addItem(gameVersion);
        ui->gameVersion->setCurrentText(modPath_->info().gameVersion());

        model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
        model_.setHorizontalHeaderItem(IdColumn, new QStandardItem(tr("Curseforge Project ID")));
        model_.setHorizontalHeaderItem(FileIdColumn, new QStandardItem(tr("Curseforge File ID")));
        model_.setHorizontalHeaderItem(RequiredColumn, new QStandardItem(tr("Required")));

        for(const auto &mod : modPath_->modList()){
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
        disableReason = tr("%1 mods disabled exporting, "
                           "because they have no corresponding project ID or file ID on Curseforge.");
        break;
    }
    case ExportManifestJsonDialog::CF_HTML:
    case ExportManifestJsonDialog::M_HTML:{
        if(mode_ == CF_HTML)
            setWindowTitle("Export modlist.html (Curseforge)");
        else
            setWindowTitle("Export modlist.html (Modrinth)");

        ui->manifestWidget->hide();
        for(auto &&gameVersion : GameVersion::mojangVersionList())
            ui->gameVersion->addItem(gameVersion);
        ui->gameVersion->setCurrentText(modPath_->info().gameVersion());

        model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
        model_.setHorizontalHeaderItem(AuthorsColumn, new QStandardItem(tr("Authors")));
        if(mode_ == CF_HTML)
            model_.setHorizontalHeaderItem(WebsiteColumn, new QStandardItem(tr("Curseforge Website")));
        else
            model_.setHorizontalHeaderItem(WebsiteColumn, new QStandardItem(tr("Modrinth Website")));

        for(const auto &mod : modPath_->modList()){
            auto nameItem = new QStandardItem;
            nameItem->setData(QVariant::fromValue(mod));
            auto authorsItem = new QStandardItem;
            auto websiteItem = new QStandardItem;
            auto list = {nameItem, authorsItem, websiteItem};
            model_.appendRow(list);

            nameItem->setText(mod->commonInfo()->name());
            nameItem->setIcon(mod->icon());
            nameItem->setCheckable(true);
            nameItem->setCheckState(Qt::Checked);

            if(mode_ == CF_HTML && mod->curseforgeMod()){
                nameItem->setText(mod->curseforgeMod()->modInfo().name());
                authorsItem->setText(mod->curseforgeMod()->modInfo().authors().join(", "));
                websiteItem->setText(mod->curseforgeMod()->modInfo().websiteUrl().toString());
            }
            if(mode_ == M_HTML && mod->modrinthMod()){
                nameItem->setText(mod->modrinthMod()->modInfo().name());
                //no author info
//                authorsItem->setText(mod->modrinthMod()->modInfo().author());
                authorsItem->setText(mod->commonInfo()->authors().join(", "));
                websiteItem->setText(mod->modrinthMod()->modInfo().websiteUrl().toString());
            }

            if(websiteItem->text().isEmpty()){
                disabledMods_ << mod;
                nameItem->setCheckState(Qt::Unchecked);
                for(auto item : list)
                    item->setEnabled(false);
            }
            if(mod->isDisabled())
                nameItem->setCheckState(Qt::Unchecked);
        }
        if(mode_ == CF_HTML)
            disableReason = tr("%1 mods disabled exporting, "
                               "because they have no corresponding project ID on Curseforge.");
        else
            disableReason = tr("%1 mods disabled exporting, "
                               "because they have no corresponding project ID on Modrinth.");
        break;
    }
    }

    if(!disabledMods_.isEmpty()){
        ui->disabledMods->setText(disableReason.arg(disabledMods_.size()));
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
    QByteArray fileContent;
    QString filePath;

    switch (mode_) {
    case ExportManifestJsonDialog::MODPACKS:
        filePath = QFileDialog::getSaveFileName(this, tr("Save your modpacks"),
                                                QDir(modPath_->info().path()).filePath(ui->name->text() + ".zip"), "*.zip");
        break;
    case ExportManifestJsonDialog::MANIFEST_JSON:{
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
        filePath = QFileDialog::getSaveFileName(this, tr("Save your manifest.json"),
                                                QDir(modPath_->info().path()).filePath("manifest.json"), "*.json");
        QJsonDocument doc(object);
        fileContent = doc.toJson();
        break;
    }
    case ExportManifestJsonDialog::CF_HTML:
    case ExportManifestJsonDialog::M_HTML:
        QStringList stringList;
        stringList << "<ul>";
        for(int row = 0; row < model_.rowCount(); row++)
            if(auto item = model_.item(row); item->checkState() == Qt::Checked){
                auto name = model_.item(row, NameColumn)->text();
                auto authors = model_.item(row, AuthorsColumn)->text();
                auto website = model_.item(row, WebsiteColumn)->text();
                stringList << QString(R"(<li><a href="%1">%2 (by %3)</a></li>)").arg(website, name, authors);
        }
        stringList << "</ul>";
        fileContent = stringList.join('\n').toUtf8();
        filePath = QFileDialog::getSaveFileName(this, tr("Save your modlist.html"),
                                                QDir(modPath_->info().path()).filePath("modlist.html"), "*.html");
        break;
    }

    if(filePath.isEmpty()) return;
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, tr("Failed Save"), tr("Can not write to file."));
        return;
    }
    file.write(fileContent);
    openFileInFolder(filePath);
    accept();
}

