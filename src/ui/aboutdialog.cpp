#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QUrl>
#include <QDesktopServices>

#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->versionText->setText(kVersion);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_toolButton_clicked()
{
    QUrl url("https://github.com/kaniol-lck/modmanager");
    QDesktopServices::openUrl(url);
}

void AboutDialog::on_toolButton_2_clicked()
{
    QUrl url("https://github.com/kaniol-lck/modmanager/issues");
    QDesktopServices::openUrl(url);
}
