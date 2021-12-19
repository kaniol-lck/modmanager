#include "githubreleaseinfowidget.h"
#include "ui_githubreleaseinfowidget.h"

#include "github/githubrelease.h"

GitHubReleaseInfoWidget::GitHubReleaseInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GitHubReleaseInfoWidget)
{
    ui->setupUi(this);
}

GitHubReleaseInfoWidget::~GitHubReleaseInfoWidget()
{
    delete ui;
}

void GitHubReleaseInfoWidget::setRelease(GitHubRelease *release)
{

    auto release_ = release;
    emit releaseChanged();
    if(!release_) return;

    ui->name->setText(release_->info().name());
    ui->body->setMarkdown(release->info().body());
}