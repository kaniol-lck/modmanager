#include "githubreleaseinfowidget.h"
#include "ui_githubreleaseinfowidget.h"

#include "github/githubrelease.h"
#include "util/smoothscrollbar.h"
#include "util/funcutil.h"

GitHubReleaseInfoWidget::GitHubReleaseInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GitHubReleaseInfoWidget)
{
    ui->setupUi(this);
    ui->body->setVerticalScrollBar(new SmoothScrollBar(this));
}

GitHubReleaseInfoWidget::~GitHubReleaseInfoWidget()
{
    delete ui;
}

void GitHubReleaseInfoWidget::setRelease(GitHubRelease *release)
{
    release_ = release;
    emit releaseChanged();
    if(!release_) return;
    connect(this, &GitHubReleaseInfoWidget::releaseChanged, disconnecter(
                connect(release_, &QObject::destroyed, this, [=]{setRelease(nullptr); })));

    ui->name->setText(release_->info().name());
    ui->body->setMarkdown(release_->info().body());
}
