#ifndef GITHUBRELEASEITEMWIDGET_H
#define GITHUBRELEASEITEMWIDGET_H

#include <QWidget>

#include "github/githubreleaseinfo.h"

class GitHubRepoBrowser;
namespace Ui {
class GitHubReleaseItemWidget;
}

class GitHubReleaseItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubReleaseItemWidget(GitHubRepoBrowser *parent, const GitHubReleaseInfo &info);
    ~GitHubReleaseItemWidget();

//private slots:
//    void on_downloadButton_clicked();

private:
    Ui::GitHubReleaseItemWidget *ui;
    GitHubRepoBrowser *browser_ = nullptr;
    GitHubReleaseInfo info_;
};

#endif // GITHUBRELEASEITEMWIDGET_H
