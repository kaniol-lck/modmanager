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
    ui->tagName->setText(info.tagName());
    ui->body->setMarkdown(info.body());
    ui->createDateTime->setText(tr("Created"));
    ui->createDateTime->setDateTime(info.created());
    ui->publishDateTime->setText(tr("Published"));
    ui->publishDateTime->setDateTime(info.published());
    ui->prerelease->setVisible(info.prerelease());
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

