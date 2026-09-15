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

void AddDownloadDialog::on_downloadUrl_textChanged(const QString &arg1)
{
    // 原来只判断 QUrl(...).isValid()：Qt 对空串、纯相对路径（"abc"）都会说"有效"，
    // 于是按钮几乎一直可点，可以提交一条实际没有下载地址的任务
    // （提交后 addUri 必然失败，用户在下载列表里只看到一条莫名失败/空转的任务）。
    const auto text = arg1.trimmed();
    const QUrl url(text);
    static const QStringList supportedSchemes = {"http", "https", "ftp", "sftp", "magnet"};
    const bool valid = !text.isEmpty() && url.isValid() && supportedSchemes.contains(url.scheme());
    if(auto &&buttons = ui->buttonBox->buttons(); !buttons.isEmpty())
        buttons.at(0)->setEnabled(valid);
}

void AddDownloadDialog::on_pasteButton_clicked()
{
    ui->downloadUrl->setText(QApplication::clipboard()->text());
}
