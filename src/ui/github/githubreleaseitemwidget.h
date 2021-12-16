#ifndef GITHUBRELEASEITEMWIDGET_H
#define GITHUBRELEASEITEMWIDGET_H

#include <QWidget>

#include "github/githubreleaseinfo.h"

namespace Ui {
class GitHubReleaseItemWidget;
}

class GitHubReleaseItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubReleaseItemWidget(QWidget *parent, const GitHubReleaseInfo &info);
    ~GitHubReleaseItemWidget();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::GitHubReleaseItemWidget *ui;
    GitHubReleaseInfo info_;
};

#endif // GITHUBRELEASEITEMWIDGET_H
