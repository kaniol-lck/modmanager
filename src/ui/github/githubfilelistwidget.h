#ifndef GITHUBFILELISTWIDGET_H
#define GITHUBFILELISTWIDGET_H

#include "ui/dockwidgetcontent.h"

class GitHubRelease;

class QStandardItemModel;
namespace Ui {
class GitHubFileListWidget;
}

class GitHubFileListWidget : public DockWidgetContent
{
    Q_OBJECT

public:
    explicit GitHubFileListWidget(QWidget *parent = nullptr);
    ~GitHubFileListWidget();

    void setRelease(GitHubRelease *release);

signals:
    void modChanged();

private slots:
    void updateFileList();
    void updateIndexWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::GitHubFileListWidget *ui;
    QStandardItemModel *model_;
    GitHubRelease *release_;
};

#endif // GITHUBFILELISTWIDGET_H
