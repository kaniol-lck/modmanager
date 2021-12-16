#ifndef GITHUBFILEITEMWIDGET_H
#define GITHUBFILEITEMWIDGET_H

#include <QWidget>

#include "github/githubfileinfo.h"

namespace Ui {
class GitHubFileItemWidget;
}

class GitHubFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubFileItemWidget(QWidget *parent, const GitHubFileInfo &info);
    ~GitHubFileItemWidget();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::GitHubFileItemWidget *ui;
    GitHubFileInfo info_;
};

#endif // GITHUBFILEITEMWIDGET_H
