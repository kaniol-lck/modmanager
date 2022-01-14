#include "importcurseforgemodpackdialog.h"
#include "ui_importcurseforgemodpackdialog.h"

#include <QJsonParseError>
#include <QProgressBar>
#include <QStandardItemModel>
#include <quazipfile.h>

#include "ui/curseforge/curseforgemoddialog.h"
#include "curseforge/curseforgeapi.h"
#include "curseforge/curseforgemod.h"
#include "gameversion.h"
#include "util/tutil.hpp"

ImportCurseforgeModpackDialog::ImportCurseforgeModpackDialog(QWidget *parent, const QString &fileName) :
    QDialog(parent),
    ui(new Ui::ImportCurseforgeModpackDialog),
    model_(new QStandardItemModel(this)),
    zip_(fileName)
{
    ui->setupUi(this);
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
    model_->setHorizontalHeaderItem(0, new QStandardItem(tr("Mod Name")));
    model_->setHorizontalHeaderItem(1, new QStandardItem(tr("Version")));
    model_->setHorizontalHeaderItem(2, new QStandardItem(tr("File Name")));
    model_->setHorizontalHeaderItem(3, new QStandardItem(tr("Required")));
    for(auto &&file : value(result, "files").toList()){
        auto projectID = value(file, "projectID").toInt();
        bool required = value(file, "required").toBool();
        auto curseforgeModItem = new QStandardItem(tr("Loading..."));
        auto mod = new CurseforgeMod(this, projectID);
        curseforgeModItem->setData(QVariant::fromValue(mod));
        curseforgeModItem->setCheckable(!required);
        curseforgeModItem->setCheckState(Qt::Checked);
        auto curseforgeFileItem = new QStandardItem(tr("Loading..."));
        auto fileNameItem = new QStandardItem;
        auto requiredItem = new QStandardItem;
        mod->acquireBasicInfo();
        connect(mod, &CurseforgeMod::basicInfoReady, this, [=]{
            curseforgeModItem->setText(mod->modInfo().name());
            mod->acquireIcon();
        });
        connect(mod, &CurseforgeMod::iconReady, this, [=]{
            curseforgeModItem->setIcon(QIcon(mod->modInfo().icon().scaled(96, 96, Qt::KeepAspectRatio)));
        });
        auto fileID = value(file, "fileID").toInt();
        auto fileInfoReply = CurseforgeAPI::api()->getFileInfo(projectID, fileID).asShared();
        fileInfoReply->setOnFinished(this, [=](const CurseforgeFileInfo &fileInfo){
            curseforgeFileItem->setText(fileInfo.displayName());
            fileNameItem->setText(fileInfo.fileName());
        });
        fileInfoReplies_ << fileInfoReply;
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

