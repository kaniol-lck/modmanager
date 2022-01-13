#include "githubreleaseitemwidget.h"
#include "githubrepobrowser.h"
#include "ui_githubreleaseitemwidget.h"

#include <QScrollBar>

#include "util/funcutil.h"

GitHubReleaseItemWidget::GitHubReleaseItemWidget(GitHubRepoBrowser *parent, const GitHubReleaseInfo &info) :
    QWidget(parent),
    ui(new Ui::GitHubReleaseItemWidget),
    browser_(parent),
    info_(info)
{
    ui->setupUi(this);
    ui->name->setProperty("class", "Title");
    setProperty("class", "GitHubReleaseItemWidget");
    ui->prerelease->setProperty("class", "PreRelease");
    ui->tagName->setProperty("class", "TagName");
    ui->body->setProperty("class", "Description");
    ui->body->verticalScrollBar()->setDisabled(true);
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
    setAttribute(Qt::WA_StyledBackground, true);
}

GitHubReleaseItemWidget::~GitHubReleaseItemWidget()
{
    delete ui;
}

//void GitHubReleaseItemWidget::on_downloadButton_clicked()
//{

//}
