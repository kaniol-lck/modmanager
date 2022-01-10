#ifndef GITHUBFILELISTWIDGET_H
#define GITHUBFILELISTWIDGET_H

#include <QWidget>

class GitHubRelease;
class QStandardItemModel;
class GitHubRepoBrowser;
class DownloadPathSelectMenu;
namespace Ui {
class GitHubFileListWidget;
}

class GitHubFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubFileListWidget(GitHubRepoBrowser *parent);
    explicit GitHubFileListWidget(QWidget *parent = nullptr);
    ~GitHubFileListWidget();

    void setRelease(GitHubRelease *release);
    void setBrowser(GitHubRepoBrowser *newBrowser);

    DownloadPathSelectMenu *downloadPathSelectMenu() const;

signals:
    void releaseChanged();

private slots:
    void updateFileList();
    void updateIndexWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::GitHubFileListWidget *ui;
    GitHubRepoBrowser *browser_ = nullptr;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel *model_;
    GitHubRelease *release_;
};

#endif // GITHUBFILELISTWIDGET_H
