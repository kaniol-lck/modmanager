#include "githubreleaseitemwidget.h"
#include "ui_githubreleaseitemwidget.h"

#include "util/funcutil.h"

GitHubReleaseItemWidget::GitHubReleaseItemWidget(QWidget *parent, const GitHubReleaseInfo &info) :
    QWidget(parent),
    ui(new Ui::GitHubReleaseItemWidget),
    info_(info)
{
    ui->setupUi(this);
    ui->name->setText(info.name());
    ui->body->setMarkdown(info.body());
    ui->downloadProgress->setVisible(false);
    ui->downloadSpeedText->setVisible(false);
    ui->downloadButton->setVisible(false);
//    ui->downloadSpeedText->setText(sizeConvert(info.size()));
}

GitHubReleaseItemWidget::~GitHubReleaseItemWidget()
{
    delete ui;
}

void GitHubReleaseItemWidget::on_downloadButton_clicked()
{

}

