#include "modmanager.h"
#include "modmanagerupdatedialog.h"
#include "ui_modmanagerupdatedialog.h"

#include <QSysInfo>
#include  <cpp-semver.hpp>

#include "github/githubapi.h"
#include "version.h"
#include "util/funcutil.h"

ModManagerUpdateDialog::ModManagerUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModManagerUpdateDialog)
{
    ui->setupUi(this);
}

ModManagerUpdateDialog::~ModManagerUpdateDialog()
{
    delete ui;
}

void ModManagerUpdateDialog::checkVersion()
{
    checkUpdateReply_ = GitHubAPI::api()->getReleases(ModManager::githubInfo(), 1).asUnique();
    checkUpdateReply_->setOnFinished(this, [=](const QList<GitHubReleaseInfo> &list){
        auto latesteRelease = list.first();
        auto tagName = latesteRelease.tagName();
        if(tagName.contains("dev") && Config().getAutoCheckModManagerUpdate() != Config::NoUpdate)
            return;
        try{
            if(semver::gt(tagName.toStdString(), kVersion)){
                updateRelease_ = latesteRelease;
                setWindowTitle(updateRelease_.tagName());
                ui->name->setText(updateRelease_.name());
                ui->body->setMarkdown(updateRelease_.body());
                for(auto &asset : updateRelease_.assets()){
#ifdef Q_OS_WIN
                    if(QSysInfo::WordSize == 64 && !asset.name().contains("x64"))
                        continue;
                    if(QSysInfo::WordSize != 64 && asset.name().contains("x64"))
                        continue;
#endif
                    if(!installerSuffix().isEmpty() && asset.name().endsWith(installerSuffix())){
                        updateFile_ = asset;
                        break;
                    }
                }
                if(!updateFile_.name().isEmpty())
                    ui->downloadFile->setText(tr("Update File: %1").arg(updateFile_.name()));
                else
                    ui->downloadFile->setText(tr("No auto update available for your platform, please update it manually."));
                emit updateChecked(true);
            } else{
                emit updateChecked(false);
            }
        } catch (...) {
            return;
        }
    });
}

const QString &ModManagerUpdateDialog::updateVersion() const
{
    return updateRelease_.tagName();
}

const GitHubReleaseInfo &ModManagerUpdateDialog::updateRelease() const
{
    return updateRelease_;
}

void ModManagerUpdateDialog::on_ModManagerUpdateDialog_accepted()
{
    if(updateFile_.name().isEmpty()) return;
    emit updateConfirmed(updateFile_);
}

void ModManagerUpdateDialog::on_pushButton_clicked()
{
    Config().setAutoCheckModManagerUpdate(Config::NoUpdate);
    close();
}

