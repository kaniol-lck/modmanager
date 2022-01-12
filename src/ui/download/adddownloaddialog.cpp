#include "adddownloaddialog.h"
#include "ui_adddownloaddialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QClipboard>

#include "download/downloadmanager.h"
#include "config.hpp"

AddDownloadDialog::AddDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDownloadDialog)
{
    ui->setupUi(this);
    ui->downloadPath->setText(Config().getDownloadPath());
    on_downloadUrl_textChanged();
}

AddDownloadDialog::~AddDownloadDialog()
{
    delete ui;
}

void AddDownloadDialog::on_selectDownloadPathButton_clicked()
{
    auto path = QFileDialog::getExistingDirectory(this, tr("Select Download Path..."), ui->downloadPath->text());
    if(path.isEmpty()) return;
    ui->downloadPath->setText(path);
}

void AddDownloadDialog::on_AddDownloadDialog_accepted()
{
    DownloadFileInfo info(
                ui->downloadUrl->text(),
                ui->downloadPath->text());
    DownloadManager::manager()->download(info);
}

void AddDownloadDialog::on_downloadUrl_textChanged(const QString &arg1[[maybe_unused]])
{
    ui->buttonBox->buttons().at(0)->setEnabled(QUrl(ui->downloadUrl->text()).isValid());
}

void AddDownloadDialog::on_pasteButton_clicked()
{
    ui->downloadUrl->setText(QApplication::clipboard()->text());
}
