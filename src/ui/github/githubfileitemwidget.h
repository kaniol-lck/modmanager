#ifndef GITHUBFILEITEMWIDGET_H
#define GITHUBFILEITEMWIDGET_H

#include <QWidget>

#include "github/githubfileinfo.h"

class GitHubFileListWidget;
namespace Ui {
class GitHubFileItemWidget;
}

class GitHubFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubFileItemWidget(GitHubFileListWidget *parent, const GitHubFileInfo &info);
    ~GitHubFileItemWidget();

public slots:
    void onDownloadPathChanged();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::GitHubFileItemWidget *ui;
    GitHubFileListWidget *fileList_ = nullptr;
    GitHubFileInfo info_;
};

#endif // GITHUBFILEITEMWIDGET_H
