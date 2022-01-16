#include "importcurseforgemodpackdialog.h"
#include "ui_importcurseforgemodpackdialog.h"

#include <QFileDialog>
#include <QJsonParseError>
#include <QProgressBar>
#include <QStandardItemModel>
#include <quazipfile.h>
#include <JlCompress.h>

#include "ui/curseforge/curseforgemoddialog.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "curseforge/curseforgefile.h"
#include "gameversion.h"
#include "util/tutil.hpp"
#include "config.hpp"
#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"

ImportCurseforgeModpackDialog::ImportCurseforgeModpackDialog(QWidget *parent, const QString &fileName) :
    QDialog(parent),
    ui(new Ui::ImportCurseforgeModpackDialog),
    model_(new QStandardItemModel(this)),
    zip_(fileName)
{
    ui->setupUi(this);
    ui->name->setProperty("class", "Title");
    ui->treeView->setModel(model_);
    for(auto &&gameVersion : GameVersion::mojangVersionList())
        ui->gameVersion->addItem(gameVersion);

    QuaZipFile zipFile(&zip_);
    if(!zip_.open(QuaZip::mdUnzip)) return;
    zip_.setCurrentFile("manifest.json");

    if(!zipFile.open(QIODevice::ReadOnly)) return;
    QByteArray json = zipFile.readAll();
    zipFile.close();

    //parse json
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug("%s", error.errorString().toUtf8().constData());
        return;
    }
    if(!jsonDocument.isObject()) return;
    QVariant result = jsonDocument.toVariant();
    auto name = value(result, "name").toString();
    ui->savePath->setText(QDir(Config().getCommonPath()).absoluteFilePath(name));
    setWindowTitle(tr("Import Modpack: %1").arg(name));
    ui->name->setText(name);
    auto version = value(result, "version").toString();
    ui->version->setText(version);
    auto author = value(result, "author").toString();
    ui->author->setText(author);
    auto minecraftVariant = value(result, "minecraft");
    if(contains(minecraftVariant, "version")){
        ui->gameVersion->setCurrentText(value(minecraftVariant, "version").toString());
        ui->gameVersion->setEnabled(false);
    }
    overrides_ = value(result, "overrides").toString();
    model_->setHorizontalHeaderItem(0, new QStandardItem(tr("Mod Name")));
    model_->setHorizontalHeaderItem(1, new QStandardItem(tr("Version")));
    model_->setHorizontalHeaderItem(2, new QStandardItem(tr("File Name")));
    model_->setHorizontalHeaderItem(3, new QStandardItem(tr("Required")));
    for(auto &&fileVariant : value(result, "files").toList()){
        auto projectID = value(fileVariant, "projectID").toInt();
        auto fileID = value(fileVariant, "fileID").toInt();
        bool required = value(fileVariant, "required").toBool();
        auto curseforgeModItem = new QStandardItem(tr("Loading..."));
        auto mod = new CurseforgeMod(this, projectID);
        curseforgeModItem->setData(QVariant::fromValue(mod));
        curseforgeModItem->setCheckable(!required);
        curseforgeModItem->setCheckState(Qt::Checked);
        auto curseforgeFileItem = new QStandardItem(tr("Loading..."));
        auto file = new CurseforgeFile(this, projectID, fileID);
        curseforgeFileItem->setData(QVariant::fromValue(file));
        auto fileNameItem = new QStandardItem;
        auto requiredItem = new QStandardItem;
        mod->acquireBasicInfo();
        connect(mod, &CurseforgeMod::basicInfoReady, this, [=]{
            curseforgeModItem->setText(mod->modInfo().name());
        });
        connect(mod, &CurseforgeMod::basicInfoReady, &CurseforgeMod::acquireIcon);
        connect(mod, &CurseforgeMod::iconReady, this, [=]{
            curseforgeModItem->setIcon(QIcon(mod->modInfo().icon().scaled(96, 96, Qt::KeepAspectRatio)));
        });
        file->acquireFileInfo();
        connect(file, &CurseforgeFile::infoReady, this, [=]{
            curseforgeFileItem->setText(file->info().displayName());
            fileNameItem->setText(file->info().fileName());
        });
        requiredItem->setCheckable(false);
        requiredItem->setCheckState(required? Qt::Checked : Qt::Unchecked);
        model_->appendRow({ curseforgeModItem, curseforgeFileItem, fileNameItem, requiredItem });
    }
    fileValid_ = model_->rowCount();
}

ImportCurseforgeModpackDialog::~ImportCurseforgeModpackDialog()
{
    delete ui;
}

bool ImportCurseforgeModpackDialog::fileValid() const
{
    return fileValid_;
}

void ImportCurseforgeModpackDialog::on_treeView_doubleClicked(const QModelIndex &index)
{
    auto mod = index.siblingAtColumn(0).data(Qt::UserRole + 1).value<CurseforgeMod *>();
    if(!mod) return;
    auto dialog = new CurseforgeModDialog(this, mod);
    dialog->show();
}

void ImportCurseforgeModpackDialog::on_toolButton_clicked()
{
    auto str = ui->savePath->text();
    if(str.isEmpty()) str = Config().getCommonPath();
    auto resultStr = QFileDialog::getExistingDirectory(this, tr("Select your mod directory..."), str);
    if(resultStr.isEmpty()) return;
    ui->savePath->setText(resultStr);
}

void ImportCurseforgeModpackDialog::on_ImportCurseforgeModpackDialog_accepted()
{
    if(ui->addToPath->isChecked()){
        auto path = new LocalModPath(LocalModPathInfo(ui->name->text(),
                                                      ui->savePath->text() + "/mods",
                                                      GameVersion(ui->gameVersion->currentText()),
                                                      ModLoaderType::Any));
        LocalModPathManager::manager()->addPath(path);
        path->loadMods();

        for(int row = 0; row < model_->rowCount(); row++){
            auto mod = model_->item(row, 0)->data().value<CurseforgeMod *>();
            auto file = model_->item(row, 1)->data().value<CurseforgeFile *>();
            path->downloadNewMod(mod, file);
        }
    } else{
        for(int row = 0; row < model_->rowCount(); row++){
            auto mod = model_->item(row, 0)->data().value<CurseforgeMod *>();
            auto file = model_->item(row, 1)->data().value<CurseforgeFile *>();
            DownloadManager::manager()->download(mod, file, ui->name->text(), ui->savePath->text() + "/mods");
        }
    }

    //extract
    for(auto &&fileName : zip_.getFileNameList())
        if(fileName.startsWith(overrides_ + "/") && !fileName.endsWith("/")){
            auto destFileName = ui->savePath->text() + "/" + fileName.mid(overrides_.size());
            destFileName.replace("//", "/");
            JlCompress::extractFile(zip_.getZipName(), fileName, destFileName);
        }
}
